Scene: Untitled
Entities:
  - Entity: 3284140195
    TagComponent:
      Tag: Green Square
    TransformComponent:
      Position: [0, 0, -4.80000019]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
    SpriteRendererComponent:
      Color: [0, 1, 0, 1]
  - Entity: 2732581695
    TagComponent:
      Tag: Red Square
    TransformComponent:
      Position: [1.70000005, 0, 1.10000002]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
    SpriteRendererComponent:
      Color: [1, 0, 0, 1]
  - Entity: 2290435736
    TagComponent:
      Tag: Camera
    TransformComponent:
      Position: [0, 0, 0]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
    CameraComponent:
      Camera:
        ProjectionType: 0
        PerspectiveFOV: 0.785398185
        PerspectiveNear: 0.00999999978
        PerspectiveFar: 1000
        OrthographicSize: 10
        OrthographicNear: -1
        OrthographicFar: 1
      Primary: false
      FixedAspectRatio: false
  - Entity: 1592343611
    TagComponent:
      Tag: Secondary Camera
    TransformComponent:
      Position: [1, 0, 6.69999981]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
    CameraComponent:
      Camera:
        ProjectionType: 1
        PerspectiveFOV: 0.785398185
        PerspectiveNear: 0.00999999978
        PerspectiveFar: 1000
        OrthographicSize: 10
        OrthographicNear: -1
        OrthographicFar: 1
      Primary: true
      FixedAspectRatio: false