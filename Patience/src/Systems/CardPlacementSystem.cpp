#include "CardPlacementSystem.h"

#include "Components/CardComponent.h"
#include "Components/CardSlotComponent.h"
#include "Components/PatienceComponent.h"

void CardPlacementSystem::Init()
{
	std::vector<EntityID> cardSlots = m_scene->GetRegistry().GetEntityList<CardSlotComponent>();

	for (EntityID slot : cardSlots)
	{
		Entity cardSlotEntity = { slot, m_scene };
		CardSlotComponent& cardSlotComponent = cardSlotEntity.GetComponent<CardSlotComponent>();
		if (cardSlotComponent.GetSlotType() == CardSlotComponent::SlotType::SLOT_TYPE_COLUMN)
		{
			ReleaseMonster(cardSlotEntity);
		}
	}
}

void CardPlacementSystem::Update(DeltaTime time)
{

}

void CardPlacementSystem::OnMouseMoved(int x, int y)
{
	Vec3 cursorCoords = Renderer2D::ConvertScreenToWorldSpace(x, y);
	CardComponent* hoveredCard = nullptr;
	bool bIsCardHeld = false;
	for (Entity entity : GetEntities())
	{
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& card = entity.GetComponent<CardComponent>();
		auto& cardSlot = card.GetCurrentSlot().GetComponent<CardSlotComponent>();
		card.SetIsHovered(false);

		if (card.GetIsHeld())
		{
			transform.SetWorldPosition(Vec3(cursorCoords.x, cursorCoords.y, transform.GetPosition().z) + card.GetHeldOffset());
			transform.SetPositionByLayerSection(Z_LAYER::FOREGROUND_3_LAYER, card.GetHeldOffset().GetMagnitude() / cardSlot.m_staggeredOffset.GetMagnitude(), 10);

			bIsCardHeld = true;
		}
		else
		{
			if (entity.HasComponent<BoxArea2DComponent>())
			{
				auto& ba2D = entity.GetComponent<BoxArea2DComponent>();

				if (ba2D.m_isMouseInArea)
				{
					if (!hoveredCard || hoveredCard->GetOwnerEntity().GetComponent<TransformComponent>().GetPosition().z < transform.GetPosition().z)
					{
						hoveredCard = &card;
					}
				}
			}
		}
	}

	if (hoveredCard && !bIsCardHeld)
	{
		hoveredCard->SetIsHovered(true);
	}
}

void CardPlacementSystem::OnMouseButtonPressed(int button)
{
	for (Entity entity : GetEntities())
	{
		auto& card = entity.GetComponent<CardComponent>();

		if (card.GetIsAvailable() && card.GetIsHovered() && card.CanBeHeld())
		{
			const CardSlotComponent& cardSlot = card.GetCurrentSlot().GetComponentRead<CardSlotComponent>();
			std::queue<Entity> cardSequence;
			cardSlot.GetCardSequence(card.GetOwnerEntity(), cardSequence);
			int numCardInSequnce = cardSequence.size();
			while (cardSequence.size() > 0)
			{
				Entity nextCardInSequence = cardSequence.front();
				auto& transform = nextCardInSequence.GetComponent<TransformComponent>();
				nextCardInSequence.GetComponent<CardComponent>().SetIsHeld(true);
				nextCardInSequence.GetComponent<CardComponent>().SetHeldOffset((float)(numCardInSequnce - cardSequence.size()) * cardSlot.m_staggeredOffset);
				nextCardInSequence.GetComponent<CardComponent>().SetPreviousPosition(transform.GetPosition());
				cardSequence.pop();
			}
			break;
		}
	}
}

bool compareByHeldOffset(const CardComponent* card1, const CardComponent* card2)
{
	return card1->GetHeldOffset().GetMag2() < card2->GetHeldOffset().GetMag2();
}

void CardPlacementSystem::OnMouseButtonReleased(int button)
{
	std::vector<CardComponent*> heldCards;
	int damage = 0;
	for (Entity entity : GetEntities())
	{
		auto& card = entity.GetComponent<CardComponent>();

		if (card.GetIsHeld())
		{
			heldCards.push_back(&card);
			damage += card.m_rank;
		}
	}

	if (heldCards.size() == 0)
	{
		return;
	}

	sort(heldCards.begin(), heldCards.end(), compareByHeldOffset);

	Entity originalSlot = heldCards[0]->GetCurrentSlot();

	for (auto& cardPtr : heldCards) 
	{
		CardComponent& card = *cardPtr;
		card.SetIsHeld(false);
		card.SetHeldOffset(Vec2(0.0f));
		PlaceCard(card.GetOwnerEntity(), heldCards.size() > 1, damage);

		//Ref<Tween> rotationTween = m_scene->CreateTween(card.GetOwnerEntity(), &card.GetOwnerEntity().GetComponent<TransformComponent>().m_rotation.z, 0.0f, math::PI, 10.0f, EasingType::SINE_OUT);
		//rotationTween->Start();
	}

	ReleaseMonster(originalSlot);

	/*if (heldCards.size() > 0)
	{
		DamageMonsterInColumn(heldCards[0]->GetCurrentSlot());
	}*/
}

void CardPlacementSystem::PlaceCard(Entity cardEntity, bool isInSequence, int damage)
{
	EntityID currentSlot = CheckForCardSlot(cardEntity);

	if (currentSlot == INVALID_ENTITY)
	{
		CardComponent& card = cardEntity.GetComponent<CardComponent>();
		TransformComponent& cardTransform = cardEntity.GetComponent<TransformComponent>();
		cardTransform.SetPosition(Vec3(card.GetPreviousPosition()));
	}
	else
	{
		Entity slotEntity = { currentSlot, m_scene };
		const CardSlotComponent& cardSlot = slotEntity.GetComponentRead<CardSlotComponent>();
		if (cardSlot.m_cardStack.size() > 0)
		{
			Entity topCardEntity = cardSlot.m_cardStack.back();
			const CardComponent& topCard = topCardEntity.GetComponentRead<CardComponent>();
			if (topCard.m_type == CardComponent::TYPE_MONSTER && topCard.m_monsterStats.m_health > damage)
			{
				CardComponent& card = cardEntity.GetComponent<CardComponent>();
				TransformComponent& cardTransform = cardEntity.GetComponent<TransformComponent>();
				cardTransform.SetPosition(Vec3(card.GetPreviousPosition()));
			}
			else
			{
				PlaceCard(cardEntity, slotEntity, isInSequence, false, true);
			}
		}
		else
		{
			PlaceCard(cardEntity, slotEntity, isInSequence, false, true);
		}
	}
}

void CardPlacementSystem::PlaceCard(Entity cardEntity, Entity slotEntity, bool isInSequence, bool isDiscard, bool updateTransform)
{
	CardComponent& card = cardEntity.GetComponent<CardComponent>();
	TransformComponent& cardTransform = cardEntity.GetComponent<TransformComponent>();
	if (updateTransform)
	{
		TransformComponent& slotTransform = slotEntity.GetComponent<TransformComponent>();
		cardTransform.SetWorldPosition(slotTransform.GetWorldPosition());
		cardTransform.SetLayer(Z_LAYER::FOREGROUND_1_LAYER);
	}
	CardSlotComponent& cardSlot = slotEntity.GetComponent<CardSlotComponent>();

	const bool cannotAcceptSequence = isInSequence && !cardSlot.CanAcceptSequences();
		
	const bool isValidWaste = (cardSlot.GetSlotType() != CardSlotComponent::SLOT_TYPE_WASTEPILE) || isDiscard;
	if (!cannotAcceptSequence && cardSlot.CanAcceptCards() && isValidWaste && cardSlot.IsCardAllowedInSlot(card.m_rank, card.m_suit))
	{
		Entity previousCardSlot = card.GetCurrentSlot();
		previousCardSlot.GetComponent<CardSlotComponent>().RemoveCard(cardEntity);
		cardSlot.AddCard(cardEntity);
		card.SetCurrentlSlot(slotEntity);
		DamageMonster(slotEntity);
	}
	else
	{
		cardTransform.SetPosition(Vec3(card.GetPreviousPosition()));
	}
}

bool CardPlacementSystem::DamageMonster(Entity siteEntity)
{
	CardSlotComponent& cardSite = siteEntity.GetComponent<CardSlotComponent>();

	if (cardSite.GetSlotType() != CardSlotComponent::SLOT_TYPE_SITE)
	{
		return false;
	}

	CardSlotComponent* damagedMonsterSlot = (EntityID)siteEntity.GetComponentRead<CardSlotComponent>().GetMonsterSlot() == INVALID_ENTITY ? nullptr : &siteEntity.GetComponentRead<CardSlotComponent>().GetMonsterSlot().GetComponent<CardSlotComponent>();

	if (!damagedMonsterSlot || damagedMonsterSlot->m_cardStack.size() == 0)
	{
		return false;
	}

	int damage = 0;
	for (Entity cardEntity : cardSite.m_cardStack)
	{
		const CardComponent& card = cardEntity.GetComponentRead<CardComponent>();
		damage += card.m_rank;
	}

	CardComponent& monsterCard = damagedMonsterSlot->m_cardStack.back().GetComponent<CardComponent>();
	bool monsterDamaged = false;
	if (monsterCard.m_monsterStats.m_health <= damage)
	{
		if (CardSlotComponent::sm_cardSlotData.monsterDiscard != INVALID_ENTITY)
		{
			Entity entity = { CardSlotComponent::sm_cardSlotData.monsterDiscard, m_scene };

			PlaceCard(monsterCard.GetOwnerEntity(), entity, false, true, false);
			MoveCardToSlot(monsterCard.GetOwnerEntity(), entity, true);
			monsterDamaged = true;

			//SpriteRendererComponent& sprite = monsterCard.GetOwnerEntity().GetComponent<SpriteRendererComponent>();
			//auto path = Project::GetAssetFileSystemPath("textures\\CardsNew\\Backs\\CardBack1.png");
			//sprite.m_texture = Texture2D::Create(path.string());
		}
	}

	// Rearrange damagin card so they card be reused
	if (monsterDamaged)
	{
		int siteCount = cardSite.m_cardStack.size();
		for (int i = 0; i < siteCount; i++)
		{
			Entity cardEntity = cardSite.m_cardStack.back();
			//cardSite.RemoveCard(cardEntity);
			//m_scene->DestroyEntity(cardEntity);

			if (CardSlotComponent::sm_cardSlotData.mainDiscard != INVALID_ENTITY)
			{
				Entity entity = { CardSlotComponent::sm_cardSlotData.mainDiscard, m_scene };
				PlaceCard(cardEntity, entity, false, true, true);

				SpriteRendererComponent& sprite = cardEntity.GetComponent<SpriteRendererComponent>();
				auto path = Project::GetAssetFileSystemPath("textures\\CardsNew\\Backs\\CardBack0.png");
				sprite.m_texture = Texture2D::Create(path.string());
			}
		}
	}

	return monsterDamaged;
}

EntityID CardPlacementSystem::CheckForCardSlot(Entity cardEntity)
{
	EntityID currentSlot = INVALID_ENTITY;
	const CardComponent& card = cardEntity.GetComponent<CardComponent>();
	const BoxArea2DComponent& cardBoxArea = cardEntity.GetComponent<BoxArea2DComponent>();
	TransformComponent& cardTransform = cardEntity.GetComponent<TransformComponent>();
	EntityID nearestCardSlot = -1;
	float nearestSeparation = -1.0f;
	std::vector<EntityID> view = m_scene->GetRegistry().GetEntityList<CardSlotComponent>();
	for (EntityID e : view)
	{
		Entity entity = { e, m_scene };
		const BoxArea2DComponent& slotArea = entity.GetComponent<BoxArea2DComponent>();
		const TransformComponent& slotTransform = entity.GetComponent<TransformComponent>();

		Vec3 cardPos = cardTransform.GetWorldPosition() + Vec3(cardBoxArea.m_offset, 0.0f);
		Vec3 slotPos = slotTransform.GetWorldPosition() + Vec3(slotArea.m_offset, 0.0f);

		Vec2 separation = Vec2(abs(cardPos.x - slotPos.x), abs(cardPos.y - slotPos.y));
		if (separation.GetMagnitude() < nearestSeparation || nearestSeparation < 0.0f)
		{
			nearestCardSlot = entity;
			nearestSeparation = separation.GetMagnitude();
		}
	}

	bool placedInSlot = false;
	if (nearestCardSlot >= 0)
	{
		Entity entity = { nearestCardSlot, m_scene };
		const BoxArea2DComponent& slotArea = entity.GetComponent<BoxArea2DComponent>();
		TransformComponent& slotTransform = entity.GetComponent<TransformComponent>();

		Vec3 cardPos = cardTransform.GetWorldPosition() + Vec3(cardBoxArea.m_offset, 0.0f);
		Vec3 slotPos = slotTransform.GetWorldPosition() + Vec3(slotArea.m_offset, 0.0f);

		Vec2 separation = Vec2(abs(cardPos.x - slotPos.x), abs(cardPos.y - slotPos.y));

		if (separation.x < (slotArea.m_size.x + cardBoxArea.m_size.x) && separation.y < (slotArea.m_size.y + cardBoxArea.m_size.y))
		{
			currentSlot = entity;
		}
	}

	return currentSlot;
}

void CardPlacementSystem::ReleaseMonster(Entity slotEntity)
{
	CardSlotComponent& cardSlotComponent = slotEntity.GetComponent<CardSlotComponent>();

	if (cardSlotComponent.m_cardStack.size() == 0)
	{
		return;
	}

	Entity patienceEntity = { m_scene->GetRegistry().GetFirstEntity<PatienceComponent>(), m_scene };
	PatienceComponent& patienceComponent = patienceEntity.GetComponent<PatienceComponent>();

	Entity topCard = cardSlotComponent.m_cardStack.back();

	if (topCard.GetComponentRead<CardComponent>().m_type != CardComponent::Type::TYPE_MONSTER)
	{
		return;
	}

	bool releasedMonster = false;
	for (Entity monsterSlot : patienceComponent.m_monsterSlots)
	{
		CardSlotComponent& monsterSlotComponent = monsterSlot.GetComponent<CardSlotComponent>();
		if (monsterSlotComponent.m_cardStack.size() > 0)
		{
			continue;
		}

		PlaceCard(topCard, monsterSlot, false, false, true);
		releasedMonster = true;
		break;
	}

	// Recursively call to get all monsters in column
	if (releasedMonster)
	{
		ReleaseMonster(slotEntity);
	}
}

void SetCardBackSprite(Entity card)
{
	SpriteRendererComponent& sprite = card.GetComponent<SpriteRendererComponent>();
	auto path = Project::GetAssetFileSystemPath("textures\\CardsNew\\Backs\\CardBack1.png");
	sprite.m_texture = Texture2D::Create(path.string());
}

void CardPlacementSystem::MoveCardToSlot(Entity card, Entity slot, bool flipCard)
{
	TransformComponent& transform = card.GetComponent<TransformComponent>();
	const TransformComponent& slotTransform = slot.GetComponentRead<TransformComponent>();
	Vec3 final = Vec3(slotTransform.GetPosition().x, slotTransform.GetPosition().y, zLayers[FOREGROUND_3_LAYER]);

	Ref<Tween> translationTween = m_scene->CreateTween(card, &transform.GetPositionRef(), transform.GetPosition(), final, 0.6f, EasingType::SINE_OUT);
	translationTween->Start();

	if (flipCard)
	{
		TweenWaitStep waitStep(0.25f);
		Ref<Tween> rotationTween = m_scene->CreateTween(card, waitStep);

		TweenCallbackStep callbackStep(&SetCardBackSprite, card);
		rotationTween->AddTweenStep(callbackStep);
		rotationTween->Start();

		/*Ref<Tween> rotationTween = m_scene->CreateTween(card, &transform.m_rotation.y, 0.0f, (3.14f / 2.0f), 1.0f, EasingType::CUBIC_IN);
		rotationTween->AddCallbackStep(&SetCardBackSprite, card);
		rotationTween->AddTweenStep(&transform.m_rotation.y, -(3.14f / 2.0f), 0.0f, 1.0f, EasingType::CUBIC_OUT);
		rotationTween->Start();*/
	}
}