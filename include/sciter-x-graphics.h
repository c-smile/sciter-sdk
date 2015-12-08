/*
 * The Sciter Engine of Terra Informatica Software, Inc.
 * http://sciter.com
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 * (C) 2003-2015, Terra Informatica Software, Inc.
 */

 /*
 * Sciter's platform independent graphics interface. Used in custom behaviors / event handlers to draw on element's surface in native code.
 *
 * Essentially this mimics Graphics object as close as possible.
 */


#ifndef __sciter_graphin_h__
#define __sciter_graphin_h__

#include "sciter-x-types.h"

namespace gool
{
  class graphics;
  class image;
  class path;
  class text_layout;
}

typedef gool::graphics* HGFX;
typedef gool::image*    HIMG;
typedef gool::path*     HPATH;
typedef gool::text_layout* HTEXT;

typedef float REAL;

typedef REAL  POS;           // position
typedef REAL  DIM;           // dimension
typedef REAL  ANGLE;         // angle (radians)
typedef unsigned int COLOR; // COLOR

struct COLOR_STOP
{
  COLOR color;
  float offset; // 0.0 ... 1.0
};

enum GRAPHIN_RESULT
{
  GRAPHIN_PANIC = -1, // e.g. not enough memory
  GRAPHIN_OK = 0,
  GRAPHIN_BAD_PARAM = 1,  // bad parameter
  GRAPHIN_FAILURE = 2,    // operation failed, e.g. restore() without save()
  GRAPHIN_NOTSUPPORTED = 3 // the platform does not support requested feature
};

enum DRAW_PATH_MODE
{
  DRAW_FILL_ONLY = 1,
  DRAW_STROKE_ONLY = 2,
  DRAW_FILL_AND_STROKE = 3,
};

enum SCITER_LINE_JOIN_TYPE
{
  SCITER_JOIN_MITER = 0,
  SCITER_JOIN_ROUND = 1,
  SCITER_JOIN_BEVEL = 2,
  SCITER_JOIN_MITER_OR_BEVEL = 3,
};

enum SCITER_LINE_CAP_TYPE
{
  SCITER_LINE_CAP_BUTT = 0,
  SCITER_LINE_CAP_SQUARE = 1,
  SCITER_LINE_CAP_ROUND = 2,
};

enum SCITER_TEXT_ALIGNMENT
{
  TEXT_ALIGN_DEFAULT,
  TEXT_ALIGN_START,
  TEXT_ALIGN_END,
  TEXT_ALIGN_CENTER,
};

enum SCITER_TEXT_DIRECTION
{
  TEXT_DIRECTION_DEFAULT,
  TEXT_DIRECTION_LTR,
  TEXT_DIRECTION_RTL,
  TEXT_DIRECTION_TTB,
};

struct SCITER_TEXT_FORMAT
{
    LPWSTR                fontFamily;
    UINT                  fontWeight; // 100...900, 400 - normal, 700 - bold
    BOOL                  fontItalic;
    float                 fontSize;   // dips
    float                 lineHeight; // dips
    SCITER_TEXT_DIRECTION textDirection;
    SCITER_TEXT_ALIGNMENT textAlignment; // horizontal alignment
    SCITER_TEXT_ALIGNMENT lineAlignment; // a.k.a. vertical alignment for roman writing systems
    LPWSTR                localeName;
};

typedef BOOL SCAPI image_write_function(LPVOID prm, const BYTE* data, UINT data_length);

struct SciterGraphicsAPI
{
// image primitives
  GRAPHIN_RESULT
        SCFN(imageCreate)( HIMG* poutImg, UINT width, UINT height, BOOL withAlpha );

  GRAPHIN_RESULT
        SCFN(imageCreateFromPixmap)( HIMG* poutImg, UINT pixmapWidth, UINT pixmapHeight, BOOL withAlpha, const COLOR* pixmapPixels );

  GRAPHIN_RESULT
        SCFN(imageAddRef)( HIMG himg );

  GRAPHIN_RESULT
        SCFN(imageRelease)( HIMG himg );

  GRAPHIN_RESULT
        SCFN(imageGetInfo)( HIMG himg,
             UINT* width,
             UINT* height,
             BOOL* usesAlpha );

  //GRAPHIN_RESULT
  //      SCFN(imageGetPixels)( HIMG himg,
  //           image_write_function* dataReceiver );

  GRAPHIN_RESULT
        SCFN(imageClear)( HIMG himg, COLOR byColor );

  GRAPHIN_RESULT
        SCFN(imageLoad)( const BYTE* bytes, UINT num_bytes, HIMG* pout_img ); // load png/jpeg/etc. image from stream of bytes

  GRAPHIN_RESULT
        SCFN(imageSave) // save png/jpeg/etc. image to stream of bytes
          ( HIMG himg,
          image_write_function* pfn, void* prm, /* function and its param passed "as is" */
          UINT bpp /*24,32 if alpha needed*/,
          UINT quality /* png: 0, jpeg:, 10 - 100 */ );


  // SECTION: graphics primitives and drawing operations

  // create COLOR value
  COLOR
        SCFN(RGBA)(UINT red, UINT green, UINT blue, UINT alpha /*= 255*/);

  GRAPHIN_RESULT
        SCFN(gCreate)(HIMG img, HGFX* pout_gfx );

  GRAPHIN_RESULT
        SCFN(gAddRef) (HGFX gfx);

  GRAPHIN_RESULT
        SCFN(gRelease) (HGFX gfx);

// Draws line from x1,y1 to x2,y2 using current lineColor and lineGradient.
  GRAPHIN_RESULT
        SCFN(gLine) ( HGFX hgfx, POS x1, POS y1, POS x2, POS y2 );

// Draws rectangle using current lineColor/lineGradient and fillColor/fillGradient with (optional) rounded corners.
  GRAPHIN_RESULT
        SCFN(gRectangle) ( HGFX hgfx, POS x1, POS y1, POS x2, POS y2 );

// Draws rounded rectangle using current lineColor/lineGradient and fillColor/fillGradient with (optional) rounded corners.
  GRAPHIN_RESULT
        SCFN(gRoundedRectangle) ( HGFX hgfx, POS x1, POS y1, POS x2, POS y2, const DIM* radii8 /*DIM[8] - four rx/ry pairs */);

// Draws circle or ellipse using current lineColor/lineGradient and fillColor/fillGradient.
  GRAPHIN_RESULT
        SCFN(gEllipse) ( HGFX hgfx, POS x, POS y, DIM rx, DIM ry );

// Draws closed arc using current lineColor/lineGradient and fillColor/fillGradient.
  GRAPHIN_RESULT
        SCFN(gArc) ( HGFX hgfx, POS x, POS y, POS rx, POS ry, ANGLE start, ANGLE sweep );

// Draws star.
  GRAPHIN_RESULT
        SCFN(gStar) ( HGFX hgfx, POS x, POS y, DIM r1, DIM r2, ANGLE start, UINT rays );

// Closed polygon.
  GRAPHIN_RESULT
        SCFN(gPolygon) ( HGFX hgfx, const POS* xy, UINT num_points );

  // Polyline.
  GRAPHIN_RESULT
        SCFN(gPolyline) ( HGFX hgfx, const POS* xy, UINT num_points );

// SECTION: Path operations

  GRAPHIN_RESULT
        SCFN(pathCreate) ( HPATH* path );

  GRAPHIN_RESULT
        SCFN(pathAddRef) ( HPATH path );

  GRAPHIN_RESULT
        SCFN(pathRelease) ( HPATH path );

  GRAPHIN_RESULT
        SCFN(pathMoveTo) ( HPATH path, POS x, POS y, BOOL relative );

  GRAPHIN_RESULT
        SCFN(pathLineTo) ( HPATH path, POS x, POS y, BOOL relative );

  GRAPHIN_RESULT
        SCFN(pathArcTo) ( HPATH path, POS x, POS y, ANGLE angle, DIM rx, DIM ry, BOOL is_large_arc, BOOL clockwise, BOOL relative );

  GRAPHIN_RESULT
        SCFN(pathQuadraticCurveTo) ( HPATH path, POS xc, POS yc, POS x, POS y, BOOL relative );

  GRAPHIN_RESULT
        SCFN(pathBezierCurveTo) ( HPATH path, POS xc1, POS yc1, POS xc2, POS yc2, POS x, POS y, BOOL relative );

  GRAPHIN_RESULT
        SCFN(pathClosePath) ( HPATH path );

  GRAPHIN_RESULT
        SCFN(gDrawPath) ( HGFX hgfx, HPATH path, DRAW_PATH_MODE dpm );

// end of path opearations

// SECTION: affine tranformations:

  GRAPHIN_RESULT
        SCFN(gRotate) ( HGFX hgfx, ANGLE radians, POS* cx /*= 0*/, POS* cy /*= 0*/ );

  GRAPHIN_RESULT
        SCFN(gTranslate) ( HGFX hgfx, POS cx, POS cy );

  GRAPHIN_RESULT
        SCFN(gScale) ( HGFX hgfx, DIM x, DIM y );

  GRAPHIN_RESULT
        SCFN(gSkew) ( HGFX hgfx, DIM dx, DIM dy );

  // all above in one shot
  GRAPHIN_RESULT
        SCFN(gTransform) ( HGFX hgfx, POS m11, POS m12, POS m21, POS m22, POS dx, POS dy );

// end of affine tranformations.

// SECTION: state save/restore

  GRAPHIN_RESULT
        SCFN(gStateSave) ( HGFX hgfx );

  GRAPHIN_RESULT
        SCFN(gStateRestore) ( HGFX hgfx );

// end of state save/restore

// SECTION: drawing attributes

  // set line width for subsequent drawings.
  GRAPHIN_RESULT
        SCFN(gLineWidth) ( HGFX hgfx, DIM width );

  GRAPHIN_RESULT
        SCFN(gLineJoin) ( HGFX hgfx, SCITER_LINE_JOIN_TYPE type );

  GRAPHIN_RESULT
        SCFN(gLineCap) ( HGFX hgfx, SCITER_LINE_CAP_TYPE type);

  //GRAPHIN_RESULT SCFN
  //      (*gNoLine ( HGFX hgfx ) { gLineWidth(hgfx,0.0); }

  // COLOR for solid lines/strokes
  GRAPHIN_RESULT
        SCFN(gLineColor) ( HGFX hgfx, COLOR c);

  // COLOR for solid fills
  GRAPHIN_RESULT
        SCFN(gFillColor) ( HGFX hgfx, COLOR color );

//inline void
//      graphics_no_fill ( HGFX hgfx ) { graphics_fill_color(hgfx, graphics_rgbt(0,0,0,0xFF)); }

  // setup parameters of linear gradient of lines.
  GRAPHIN_RESULT
        SCFN(gLineGradientLinear)( HGFX hgfx, POS x1, POS y1, POS x2, POS y2, COLOR_STOP* stops, UINT nstops );

  // setup parameters of linear gradient of fills.
  GRAPHIN_RESULT
        SCFN(gFillGradientLinear)( HGFX hgfx, POS x1, POS y1, POS x2, POS y2, COLOR_STOP* stops, UINT nstops );

  // setup parameters of line gradient radial fills.
  GRAPHIN_RESULT
        SCFN(gLineGradientRadial)( HGFX hgfx, POS x, POS y, DIM rx, DIM ry, COLOR_STOP* stops, UINT nstops );

  // setup parameters of gradient radial fills.
  GRAPHIN_RESULT
        SCFN(gFillGradientRadial)( HGFX hgfx, POS x, POS y, DIM rx, DIM ry, COLOR_STOP* stops, UINT nstops );

  GRAPHIN_RESULT
        SCFN(gFillMode) ( HGFX hgfx, BOOL even_odd /* false - fill_non_zero */ );

// SECTION: text

  // create text layout using element's styles
  GRAPHIN_RESULT
        SCFN(textCreateForElement)(HTEXT* ptext, LPCWSTR text, UINT textLength, HELEMENT he );

  // create text layout using explicit format declaration
  GRAPHIN_RESULT
        SCFN(textCreate)(HTEXT* ptext, LPCWSTR text, UINT textLength, const SCITER_TEXT_FORMAT* format );

  GRAPHIN_RESULT
        SCFN(textGetMetrics)(HTEXT text, DIM* minWidth, DIM* maxWidth, DIM* height, DIM* ascent, DIM* descent, UINT* nLines);

  GRAPHIN_RESULT
        SCFN(textSetBox)(HTEXT text, DIM width, DIM height);

  // draw text with position (1..9 on MUMPAD) at px,py
  // Ex: gDrawText( 100,100,5) will draw text box with its center at 100,100 px
  GRAPHIN_RESULT
        SCFN(gDrawText) ( HGFX hgfx, HTEXT text, POS px, POS py, UINT position );

  // SECTION: image rendering

  // draws img onto the graphics surface with current transformation applied (scale, rotation).
  GRAPHIN_RESULT
        SCFN(gDrawImage) ( HGFX hgfx, HIMG himg, POS x, POS y,
                           DIM* w /*= 0*/, DIM* h /*= 0*/, UINT* ix /*= 0*/, UINT* iy /*= 0*/, UINT* iw /*= 0*/, UINT* ih, /*= 0*/
                           float* opacity /*= 0, if provided is in 0.0 .. 1.0*/ );

  // SECTION: coordinate space

  GRAPHIN_RESULT
        SCFN(gWorldToScreen) ( HGFX hgfx, POS* inout_x, POS* inout_y);

  //inline GRAPHIN_RESULT
  //      graphics_world_to_screen ( HGFX hgfx, POS* length)
  //{
  //   return graphics_world_to_screen ( hgfx, length, 0);
  //}

  GRAPHIN_RESULT
        SCFN(gScreenToWorld) ( HGFX hgfx, POS* inout_x, POS* inout_y);

  //inline GRAPHIN_RESULT
  //      graphics_screen_to_world ( HGFX hgfx, POS* length)
  //{
  //   return graphics_screen_to_world (hgfx, length, 0);
  //}

// SECTION: clipping

  GRAPHIN_RESULT
        SCFN(gPushClipBox) ( HGFX hgfx, POS x1, POS y1, POS x2, POS y2, float opacity /*=1.f*/);

  GRAPHIN_RESULT
        SCFN(gPushClipPath) ( HGFX hgfx, HPATH hpath, float opacity /*=1.f*/);

  // pop clip layer previously set by gPushClipBox or gPushClipPath
  GRAPHIN_RESULT
        SCFN(gPopClip) ( HGFX hgfx);

};

typedef struct SciterGraphicsAPI* LPSciterGraphicsAPI;

#endif
