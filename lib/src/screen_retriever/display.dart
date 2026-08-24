import 'dart:ui';

/// Description of a user display screen.
class Display {
  const Display({
    required this.id,
    this.name,
    required this.size,
    this.visiblePosition,
    this.visibleSize,
    this.scaleFactor,
    this.physicalBounds,
    this.physicalWorkArea,
    this.dpi,
  });

  factory Display.fromJson(Map<String, dynamic> json) {
    final sizeMap = json['size'] as Map;
    final visiblePositionMap = json['visiblePosition'] as Map?;
    final visibleSizeMap = json['visibleSize'] as Map?;

    return Display(
      id: json['id'] as String,
      name: json['name'] as String?,
      size: Size((sizeMap['width'] as num).toDouble(), (sizeMap['height'] as num).toDouble()),
      visiblePosition: visiblePositionMap != null
          ? Offset((visiblePositionMap['dx'] as num).toDouble(), (visiblePositionMap['dy'] as num).toDouble())
          : null,
      visibleSize: visibleSizeMap != null
          ? Size((visibleSizeMap['width'] as num).toDouble(), (visibleSizeMap['height'] as num).toDouble())
          : null,
      scaleFactor: json['scaleFactor'] as num?,
      physicalBounds: _rectFromJson(json['physicalBounds']),
      physicalWorkArea: _rectFromJson(json['physicalWorkArea']),
      dpi: json['dpi'] as num?,
    );
  }

  static Rect? _rectFromJson(Object? value) {
    if (value is! Map) return null;
    return Rect.fromLTWH(
      (value['x'] as num).toDouble(),
      (value['y'] as num).toDouble(),
      (value['width'] as num).toDouble(),
      (value['height'] as num).toDouble(),
    );
  }

  /// Unique identifier associated with the display.
  final String id;

  /// The name of the display.
  final String? name;

  /// The size of the display in logical pixels.
  final Size size;

  /// The visible area position of the display in logical pixels.
  /// Uses Flutter coordinate space (Y-down, origin at top-left of primary screen).
  final Offset? visiblePosition;

  /// The visible area size of the display in logical pixels.
  final Size? visibleSize;

  /// The scale factor of the display.
  final num? scaleFactor;

  /// The display's rectangle in device pixels, in virtual-desktop coordinates.
  ///
  /// Unlike [visiblePosition] and [size], which divide each display by its own
  /// scale factor, these form one coherent space across displays. Pass to
  /// `MultiViewDesktop.setPhysicalBounds` to cover a display exactly.
  ///
  /// Windows only; null elsewhere.
  final Rect? physicalBounds;

  /// [physicalBounds] minus the taskbar and other appbars. Windows only.
  final Rect? physicalWorkArea;

  /// The display's raw DPI, 96 being unscaled. Windows only.
  final num? dpi;
}
