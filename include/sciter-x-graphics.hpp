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
 *
 * C++ wrappers
 */


#ifndef __sciter_graphin_hpp__
#define __sciter_graphin_hpp__

#include "sciter-x-graphics.h"
#include "sciter-x-api.h"

#if defined(__cplusplus) && !defined( PLAIN_API_ONLY )

#include "aux-slice.h"

namespace sciter
{

  struct writer
  {
    virtual bool write( aux::bytes data ) = 0; // redefine to do actual writing of data.start/data.length
    static BOOL SCAPI image_write_function(LPVOID prm, const BYTE* data, UINT data_length)
    {
      writer* pw = (writer* )prm;
      return pw->write( aux::bytes(data,data_length) );
    }
  };

  class graphics;

  class image
  {
    friend class graphics;
  protected:
    HIMG himg;
    
    image(HIMG h): himg(h) { ; }
    
  public:

    image(): himg(0) { ; }
    image(const image& im): himg(im.himg) { if(himg) gapi()->imageAddRef(himg); }
    image& operator = (const image& im) 
    { 
      if(himg) gapi()->imageRelease(himg); 
      himg = im.himg; gapi()->imageAddRef(himg); 
      return *this;
    }
    
    bool is_valid() const { return himg != 0; }

    static image create( UINT width, UINT height, bool withAlpha )
    {
      HIMG himg = 0;
      GRAPHIN_RESULT r = gapi()->imageCreate(&himg, width, height, withAlpha ); assert(r == GRAPHIN_OK); r;
      if( himg ) 
        return image( himg );
      return image(0);
    }
    static image load( aux::bytes data ) // loads image from png or jpeg enoded data
    {
      HIMG himg;
      GRAPHIN_RESULT r = gapi()->imageLoad( data.start, data.length, &himg); assert(r == GRAPHIN_OK); r;
      if( himg )
        return image( himg );
      return image(0);
    }
    void save( writer& w, UINT quality = 0 /*JPEG qquality: 20..100, if 0 - PNG */ ) // save image as png or jpeg enoded data
    {
      GRAPHIN_RESULT r = gapi()->imageSave( himg, writer::image_write_function, &w, 24, quality ); 
      assert(r == GRAPHIN_OK); r;
    }

    /* example of use of the writer:
    struct _: public writer // helper functor
    {
      pod::byte_buffer& bb;
      inline _( pod::byte_buffer& abb ): bb(abb) {}
      inline virtual bool write( aux::bytes data ) { bb.push(data.start, data.length); return true; }
    };

    void save( pod::byte_buffer& bb, UINT quality = 0)
    {
        save(_(bb),quality);
    }*/

    ~image() 
    {
      if( himg ) {
        GRAPHIN_RESULT r = gapi()->imageRelease( himg ); assert(r == GRAPHIN_OK); r;
      }
    }
    
    bool dimensions( UINT& width, UINT& height ) {
      if( himg ) {
        BOOL usesAlpha;
        GRAPHIN_RESULT r = gapi()->imageGetInfo(himg,&width,&height,&usesAlpha); assert(r == GRAPHIN_OK); r;
        return true;
      }
      width = 0; height = 0;
      return false;
    }

    void clear(COLOR c = 0)
    {
        GRAPHIN_RESULT r = gapi()->imageClear(himg,c); 
        assert(r == GRAPHIN_OK); r;
    }

  };

  inline COLOR gcolor(UINT r, UINT g, UINT b, UINT a = 255) { return gapi()->RGBA( r, g, b, a ); }

  // graphic path object 
  class path
  {
    friend class graphics;
  protected:
    HPATH hpath;

    path(HPATH h): hpath(h) { }
  public:
    path(): hpath(0) { }
    path(const path& pa): hpath(pa.hpath) { if(hpath) gapi()->pathAddRef(hpath); }
    path& operator = (const path& pa) 
    { 
      if(hpath) gapi()->pathRelease(hpath); 
      hpath = pa.hpath; gapi()->pathAddRef(hpath); 
      return *this;
    }
    
    bool is_valid() const { return hpath != 0; }

    static path create()
    {
      HPATH hpath = 0;
      GRAPHIN_RESULT r = gapi()->pathCreate(&hpath); assert(r == GRAPHIN_OK); r;
      return path( hpath );
    }

    ~path() 
    {
      if( hpath ) {
        GRAPHIN_RESULT r = gapi()->pathRelease( hpath ); assert(r == GRAPHIN_OK); r;
      }
    }

    void move_to ( POS x, POS y, bool relative )
    {
      assert(hpath);
      GRAPHIN_RESULT r = gapi()->pathMoveTo( hpath, x, y, relative ); 
      assert(r == GRAPHIN_OK); r;
    }

    void line_to ( POS x, POS y, bool relative )
    {
      assert(hpath);
      GRAPHIN_RESULT r = gapi()->pathLineTo( hpath, x, y, relative ); 
      assert(r == GRAPHIN_OK); r;
    }

    void arc_to( POS x, POS y, ANGLE angle, POS rx, POS ry, bool is_large_arc, bool sweep_flag, bool relative )
    {
      assert(hpath);
      GRAPHIN_RESULT r = gapi()->pathArcTo( hpath, x, y, angle, rx, ry, is_large_arc, sweep_flag, relative ); 
      assert(r == GRAPHIN_OK); r;
    }

    void quadratic_curve_to ( POS xc, POS yc, POS x, POS y, bool relative )
    {
      assert(hpath);
      GRAPHIN_RESULT r = gapi()->pathQuadraticCurveTo( hpath, xc, yc, x, y, relative ); 
      assert(r == GRAPHIN_OK); r;
    }

    void bezier_curve_to ( POS xc1, POS yc1, POS xc2, POS yc2, POS x, POS y, bool relative )
    {
      assert(hpath);
      GRAPHIN_RESULT r = gapi()->pathBezierCurveTo( hpath, xc1, yc1, xc2, yc2, x, y, relative ); 
      assert(r == GRAPHIN_OK); r;
    }

    void close_path()
    {
      assert(hpath);
      GRAPHIN_RESULT r = gapi()->pathClosePath( hpath ); 
      assert(r == GRAPHIN_OK); r;
    }
  };


  class graphics
  {
    HGFX hgfx;  
  public:

    graphics( HGFX gfx ): hgfx(gfx) { if(hgfx) gapi()->gAddRef(hgfx); }
    
    ~graphics() { if(hgfx) gapi()->gRelease(hgfx); }

    // Draws line from x1,y1 to x2,y2 using current line_color and line_gradient.
    void line ( POS x1, POS y1, POS x2, POS y2 )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gLine( hgfx, x1, y1, x2, y2 ); 
      assert(r == GRAPHIN_OK); r;
    }

    // Draws rectangle using current lineColor/lineGradient and fillColor/fillGradient 
    void rectangle ( POS x1, POS y1, POS x2, POS y2 )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gRectangle( hgfx, x1, y1, x2, y2 ); 
      assert(r == GRAPHIN_OK); r;
    }

    // Draws rounded rectangle using current lineColor/lineGradient and fillColor/fillGradient with rounded corners.
    void rectangle ( POS x1, POS y1, POS x2, POS y2, DIM rAll )
    {
      DIM rad[8] = { rAll, rAll, rAll, rAll, rAll, rAll, rAll, rAll};
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gRoundedRectangle( hgfx, x1, y1, x2, y2, rad ); 
      assert(r == GRAPHIN_OK); r;
    }

    void rectangle ( POS x1, POS y1, POS x2, POS y2, DIM rTopLeft, DIM rTopRight, DIM rBottomRight, DIM rBottomLeft )
    {
      DIM rad[8] = { rTopLeft, rTopLeft, rTopRight, rTopRight, rBottomRight, rBottomRight, rBottomLeft, rBottomLeft };
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gRoundedRectangle( hgfx, x1, y1, x2, y2, rad ); 
      assert(r == GRAPHIN_OK); r;
    }


    // Draws circle or ellipse using current lineColor/lineGradient and fillColor/fillGradient.
    void ellipse ( POS x, POS y, POS rx, POS ry )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gEllipse( hgfx, x, y, rx, ry ); 
      assert(r == GRAPHIN_OK); r;
    }
    void circle ( POS x, POS y, POS radii )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gEllipse( hgfx, x, y, radii, radii ); 
      assert(r == GRAPHIN_OK); r;
    }

    // Draws closed arc using current lineColor/lineGradient and fillColor/fillGradient.
    void arc ( POS x, POS y, POS rx, POS ry, ANGLE start, ANGLE sweep )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gArc( hgfx, x, y, rx, ry, start, sweep ); 
      assert(r == GRAPHIN_OK); r;
    }

    // Draws star.
    void star ( POS x, POS y, POS r1, POS r2, ANGLE start, UINT rays )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gStar( hgfx, x, y, r1, r2, start, rays ); 
      assert(r == GRAPHIN_OK); r;
    }

    // Draws closed polygon using current lineColor/lineGradient and fillColor/fillGradient.
    void polygon ( POS* xy, UINT num_points )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gPolygon( hgfx, xy, num_points ); 
      assert(r == GRAPHIN_OK); r;
    }

    // you bet
    void polyline ( POS* xy, unsigned int num_points )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gPolyline( hgfx, xy, num_points ); 
      assert(r == GRAPHIN_OK); r;
    }

    // SECTION: Path operations

    void draw_path(const path& p, DRAW_PATH_MODE dpm)
    {
      assert(hgfx && p.hpath);
      GRAPHIN_RESULT r = gapi()->gDrawPath( hgfx, p.hpath, dpm ); 
      assert(r == GRAPHIN_OK); r;
    }

    // end of path opearations

    // SECTION: affine tranformations:

    void rotate ( ANGLE radians )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gRotate( hgfx, radians, 0, 0 ); 
      assert(r == GRAPHIN_OK); r;
    }

    void rotate ( ANGLE radians, POS center_x, POS center_y )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gRotate( hgfx, radians, &center_x, &center_y ); 
      assert(r == GRAPHIN_OK); r;
    }

    void translate ( POS cx, POS cy )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gTranslate( hgfx, cx, cy ); 
      assert(r == GRAPHIN_OK); r;
    }

    void scale ( REAL x, REAL y )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gScale( hgfx, x, y ); 
      assert(r == GRAPHIN_OK); r;
    }

    void skew ( REAL dx, REAL dy )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gSkew( hgfx, dx, dy ); 
      assert(r == GRAPHIN_OK); r;
    }

    // all above in one shot
    void transform ( POS m11, POS m12, POS m21, POS m22, POS dx, POS dy )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gTransform( hgfx, m11, m12, m21, m22, dx, dy ); 
      assert(r == GRAPHIN_OK); r;
    }

    // end of affine tranformations.

    // SECTION: state save/restore

    void state_save ( )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gStateSave( hgfx ); 
      assert(r == GRAPHIN_OK); r;
    }

    void state_restore ( )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gStateRestore( hgfx ); 
      assert(r == GRAPHIN_OK); r;
    }

    // end of state save/restore

    // SECTION: drawing attributes

    // set line width for subsequent drawings.
    void line_width ( DIM width )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gLineWidth( hgfx, width ); 
      assert(r == GRAPHIN_OK); r;
    }

    void no_line ( ) 
    { 
      line_width(0.0); 
    }

    void line_color ( COLOR c ) 
    { 
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gLineColor( hgfx, c); 
      assert(r == GRAPHIN_OK); r;
    }

    void line_cap(SCITER_LINE_CAP_TYPE ct)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gLineCap( hgfx, ct); 
      assert(r == GRAPHIN_OK); r;
    }

    void line_join(SCITER_LINE_JOIN_TYPE jt)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gLineJoin( hgfx, jt); 
      assert(r == GRAPHIN_OK); r;
    }

    // COLOR for solid fills
    void fill_color ( COLOR c )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gFillColor( hgfx, c); 
      assert(r == GRAPHIN_OK); r;
    }

    void no_fill ( ) 
    { 
      fill_color ( COLOR(0) );
    }

    // setup parameters of linear gradient of lines.
    void line_linear_gradient( POS x1, POS y1, POS x2, POS y2, COLOR_STOP* stops, UINT nstops )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gLineGradientLinear( hgfx, x1, y1, x2, y2, stops, nstops ); 
      assert(r == GRAPHIN_OK); r;
    }
    void line_linear_gradient( POS x1, POS y1, POS x2, POS y2, COLOR c1, COLOR c2 )
    {
      COLOR_STOP stops[2] = { {c1, 0.0}, {c2, 1.0} };
      fill_linear_gradient( x1, y1, x2, y2, stops, 2 );
    }

    // setup parameters of linear gradient of fills.
    void fill_linear_gradient( POS x1, POS y1, POS x2, POS y2, COLOR_STOP* stops, UINT nstops )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gFillGradientLinear( hgfx, x1, y1, x2, y2, stops, nstops); 
      assert(r == GRAPHIN_OK); r;
    }

    void fill_linear_gradient( POS x1, POS y1, POS x2, POS y2, COLOR c1, COLOR c2 )
    {
      COLOR_STOP stops[2] = { {c1, 0.0}, {c2, 1.0} };
      fill_linear_gradient( x1, y1, x2, y2, stops, 2 );
    }

    // setup parameters of line gradient radial fills.
    void line_radial_gradient( POS x, POS y, DIM radiix, DIM radiiy, COLOR_STOP* stops, UINT nstops )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gFillGradientRadial( hgfx, x, y, radiix, radiiy, stops, nstops); 
      assert(r == GRAPHIN_OK); r;
    }

    void line_radial_gradient( POS x, POS y, DIM radiix, DIM radiiy, COLOR c1, COLOR c2 )
    {
      COLOR_STOP stops[2] = { {c1, 0.0}, {c2, 1.0} };
      line_radial_gradient( x, y, radiix,radiiy, stops, 2 );
    }

    // setup parameters of gradient radial fills.
    void fill_radial_gradient( POS x, POS y, DIM radiix, DIM radiiy, COLOR_STOP* stops, UINT nstops )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gFillGradientRadial( hgfx, x, y, radiix, radiiy, stops, nstops); 
      assert(r == GRAPHIN_OK); r;
    }

    void fill_radial_gradient( POS x, POS y, DIM radiix, DIM radiiy, COLOR c1, COLOR c2 )
    {
      COLOR_STOP stops[2] = { {c1, 0.0}, {c2, 1.0} };
      fill_radial_gradient( x, y, radiix, radiiy, stops, 2 );
    }
    
    void fill_mode( bool even_odd /* false - fill_non_zero */ )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gFillMode( hgfx, even_odd ); 
      assert(r == GRAPHIN_OK); r;
    }

    // SECTION: text
/*
    // define font attributes for all subsequent text operations.
    void font(  const char* name, DIM size, bool bold = false, bool italic = false, ANGLE angle = 0)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gSetFont( hgfx, name, size, bold, italic, angle ); 
      assert(r == GRAPHIN_OK); r;
    }

    // draw text at x,y with text alignment
    void text(POS x, POS y, aux::wchars t)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gText( hgfx, x, y, t.start, t.length ); 
      assert(r == GRAPHIN_OK); r;
    }

    // calculates width of the text string
    DIM text_width( aux::wchars t )
    {
      DIM width;
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gTextWidth( hgfx, t.start, t.length, &width ); 
      assert(r == GRAPHIN_OK); r;
      return width;
    }

    // returns height and ascent of the font.
    void font_metrics( DIM& height, DIM& ascent )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gFontMetrics( hgfx, &height, &ascent ); 
      assert(r == GRAPHIN_OK); r;
    }

    // set text alignments
    void text_alignment( SCITER_TEXT_ALIGNMENT x, SCITER_TEXT_ALIGNMENT y)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gTextAlignment( hgfx, x, y ); 
      assert(r == GRAPHIN_OK); r;
    }
*/

    // SECTION: image rendering

    // draws img onto the graphics surface with current transformation applied (scale, rotation). expensive
    void draw_image ( image* pimg, POS x, POS y, DIM w, DIM h, UINT ix, UINT iy, UINT iw, UINT ih )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gDrawImage( hgfx, pimg->himg, x, y, &w, &h, &ix, &iy, &iw, &ih, 0 ); 
      assert(r == GRAPHIN_OK); r;
    }
    // draws whole img onto the graphics surface with current transformation applied (scale, rotation). expensive
    void draw_image ( image* pimg, POS x, POS y )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gDrawImage( hgfx, pimg->himg, x, y, 0, 0, 0, 0, 0, 0, 0 ); 
      assert(r == GRAPHIN_OK); r;
    }
    // blends image bits with bits of the surface. no affine transformations. less expensive
    void blend_image ( image* pimg, POS x, POS y, float opacity, UINT ix, UINT iy, UINT iw, UINT ih )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gDrawImage( hgfx, pimg->himg, x, y, 0, 0, &ix, &iy, &iw, &ih, &opacity ); 
      assert(r == GRAPHIN_OK); r;
    }
    // blends image bits with bits of the surface. no affine transformations. less expensive
    void blend_image ( image* pimg, POS x, POS y, float opacity )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gDrawImage( hgfx, pimg->himg, x, y, 0, 0, 0, 0, 0, 0, &opacity ); 
      assert(r == GRAPHIN_OK); r;
    }

    // SECTION: coordinate space

    void world_to_screen ( POS& inout_x, POS& inout_y)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gWorldToScreen( hgfx, &inout_x, &inout_y ); 
      assert(r == GRAPHIN_OK); r;
    }

    void world_to_screen ( DIM& inout_length)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gWorldToScreen( hgfx, &inout_length, 0 ); 
      assert(r == GRAPHIN_OK); r;
    }

    void screen_to_world ( POS& inout_x, POS& inout_y)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gScreenToWorld( hgfx, &inout_x, &inout_y ); 
      assert(r == GRAPHIN_OK); r;
    }

    void screen_to_world ( DIM& inout_length)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gScreenToWorld( hgfx, &inout_length, 0 ); 
      assert(r == GRAPHIN_OK); r;
    }

    // SECTION: clipping

    void push_clip_box ( POS x1, POS y1, POS x2, POS y2, float opacity = 1.0)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gPushClipBox( hgfx, x1, y1, x2, y2, opacity ); 
      assert(r == GRAPHIN_OK); r;
    }

    void push_clip_path ( const path& p, float opacity = 1.0)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gPushClipPath( hgfx, p.hpath , opacity ); 
      assert(r == GRAPHIN_OK); r;
    }

    void pop_clip ()
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gPopClip( hgfx); 
      assert(r == GRAPHIN_OK); r;
    }
  };

}

#endif //defined(__cplusplus) && !defined( PLAIN_API_ONLY )


#endif


