#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <assert.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TYPES_H
#include FT_TRUETYPE_TABLES_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H
#include FT_OUTLINE_H
#include FT_TRIGONOMETRY_H
#include FT_MODULE_H
#include FT_WINFONTS_H
#ifdef FT_LCD_FILTER_H
#include FT_LCD_FILTER_H
#endif

/*
 * gcc -shared -Wl,--kill-at,--enable-stdcall-fixup,-s -D_UNICODE -DUNICODE
 * -O gdixp.def -o gdixp.dll gdixp.c
 */
 
 // FIXME freetye section need to be initialize
 
HINSTANCE hmodule = NULL;
HINSTANCE gdi = NULL;
BOOL WINAPI DllMain(HINSTANCE hInst,DWORD reason,LPVOID lpvReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        hmodule = hInst;
        gdi = LoadLibrary(_T("GDI32.dll"));
        if (!gdi) return FALSE;
    }
    if (reason == DLL_PROCESS_DETACH) FreeLibrary(gdi);
    return TRUE;
}

//redefine funcs
#define strlenW wcslen

//supress msg
#define TRACE(...) do { } while(0)
#define WARN(...) do { } while(0)
#define FIXME(...) do { } while(0)
#define ERR(...) do { } while(0)

// Wine

/*
 * GDI font objects
 *
 * Copyright 1993 Alexandre Julliard
 *           1997 Alex Korobka
 * Copyright 2002,2003 Shachar Shemesh
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

struct gdi_dc_funcs;
struct opengl_funcs;

typedef struct gdi_physdev
{
    const struct gdi_dc_funcs *funcs;
    struct gdi_physdev        *next;
    HDC                        hdc;
} *PHYSDEV;

struct bitblt_coords
{
    int  log_x;     /* original position and size, in logical coords */
    int  log_y;
    int  log_width;
    int  log_height;
    int  x;         /* mapped position and size, in device coords */
    int  y;
    int  width;
    int  height;
    RECT visrect;   /* rectangle clipped to the visible part, in device coords */
    DWORD layout;   /* DC layout */
};

struct gdi_image_bits
{
    void   *ptr;       /* pointer to the bits */
    BOOL    is_copy;   /* whether this is a copy of the bits that can be modified */
    void  (*free)(struct gdi_image_bits *);  /* callback for freeing the bits */
    void   *param;     /* extra parameter for callback private use */
};

struct brush_pattern
{
    BITMAPINFO           *info;     /* DIB info */
    struct gdi_image_bits bits;     /* DIB bits */
    UINT                  usage;    /* color usage for DIB info */
};

struct gdi_dc_funcs
{
    INT      (*pAbortDoc)(PHYSDEV);
    BOOL     (*pAbortPath)(PHYSDEV);
    BOOL     (*pAlphaBlend)(PHYSDEV,struct bitblt_coords*,PHYSDEV,struct bitblt_coords*,BLENDFUNCTION);
    BOOL     (*pAngleArc)(PHYSDEV,INT,INT,DWORD,FLOAT,FLOAT);
    BOOL     (*pArc)(PHYSDEV,INT,INT,INT,INT,INT,INT,INT,INT);
    BOOL     (*pArcTo)(PHYSDEV,INT,INT,INT,INT,INT,INT,INT,INT);
    BOOL     (*pBeginPath)(PHYSDEV);
    DWORD    (*pBlendImage)(PHYSDEV,BITMAPINFO*,const struct gdi_image_bits*,struct bitblt_coords*,struct bitblt_coords*,BLENDFUNCTION);
    BOOL     (*pChord)(PHYSDEV,INT,INT,INT,INT,INT,INT,INT,INT);
    BOOL     (*pCloseFigure)(PHYSDEV);
    BOOL     (*pCreateCompatibleDC)(PHYSDEV,PHYSDEV*);
    BOOL     (*pCreateDC)(PHYSDEV*,LPCWSTR,LPCWSTR,LPCWSTR,const DEVMODEW*);
    BOOL     (*pDeleteDC)(PHYSDEV);
    BOOL     (*pDeleteObject)(PHYSDEV,HGDIOBJ);
    DWORD    (*pDeviceCapabilities)(LPSTR,LPCSTR,LPCSTR,WORD,LPSTR,LPDEVMODEA);
    BOOL     (*pEllipse)(PHYSDEV,INT,INT,INT,INT);
    INT      (*pEndDoc)(PHYSDEV);
    INT      (*pEndPage)(PHYSDEV);
    BOOL     (*pEndPath)(PHYSDEV);
    BOOL     (*pEnumFonts)(PHYSDEV,LPLOGFONTW,FONTENUMPROCW,LPARAM);
    INT      (*pEnumICMProfiles)(PHYSDEV,ICMENUMPROCW,LPARAM);
    INT      (*pExcludeClipRect)(PHYSDEV,INT,INT,INT,INT);
    INT      (*pExtDeviceMode)(LPSTR,HWND,LPDEVMODEA,LPSTR,LPSTR,LPDEVMODEA,LPSTR,DWORD);
    INT      (*pExtEscape)(PHYSDEV,INT,INT,LPCVOID,INT,LPVOID);
    BOOL     (*pExtFloodFill)(PHYSDEV,INT,INT,COLORREF,UINT);
    INT      (*pExtSelectClipRgn)(PHYSDEV,HRGN,INT);
    BOOL     (*pExtTextOut)(PHYSDEV,INT,INT,UINT,const RECT*,LPCWSTR,UINT,const INT*);
    BOOL     (*pFillPath)(PHYSDEV);
    BOOL     (*pFillRgn)(PHYSDEV,HRGN,HBRUSH);
    BOOL     (*pFlattenPath)(PHYSDEV);
    BOOL     (*pFontIsLinked)(PHYSDEV);
    BOOL     (*pFrameRgn)(PHYSDEV,HRGN,HBRUSH,INT,INT);
    BOOL     (*pGdiComment)(PHYSDEV,UINT,const BYTE*);
    UINT     (*pGetBoundsRect)(PHYSDEV,RECT*,UINT);
    BOOL     (*pGetCharABCWidths)(PHYSDEV,UINT,UINT,LPABC);
    BOOL     (*pGetCharABCWidthsI)(PHYSDEV,UINT,UINT,WORD*,LPABC);
    BOOL     (*pGetCharWidth)(PHYSDEV,UINT,UINT,LPINT);
    INT      (*pGetDeviceCaps)(PHYSDEV,INT);
    BOOL     (*pGetDeviceGammaRamp)(PHYSDEV,LPVOID);
    DWORD    (*pGetFontData)(PHYSDEV,DWORD,DWORD,LPVOID,DWORD);
    BOOL     (*pGetFontRealizationInfo)(PHYSDEV,void*);
    DWORD    (*pGetFontUnicodeRanges)(PHYSDEV,LPGLYPHSET);
    DWORD    (*pGetGlyphIndices)(PHYSDEV,LPCWSTR,INT,LPWORD,DWORD);
    DWORD    (*pGetGlyphOutline)(PHYSDEV,UINT,UINT,LPGLYPHMETRICS,DWORD,LPVOID,const MAT2*);
    BOOL     (*pGetICMProfile)(PHYSDEV,LPDWORD,LPWSTR);
    DWORD    (*pGetImage)(PHYSDEV,BITMAPINFO*,struct gdi_image_bits*,struct bitblt_coords*);
    DWORD    (*pGetKerningPairs)(PHYSDEV,DWORD,LPKERNINGPAIR);
    COLORREF (*pGetNearestColor)(PHYSDEV,COLORREF);
    UINT     (*pGetOutlineTextMetrics)(PHYSDEV,UINT,LPOUTLINETEXTMETRICW);
    COLORREF (*pGetPixel)(PHYSDEV,INT,INT);
    UINT     (*pGetSystemPaletteEntries)(PHYSDEV,UINT,UINT,LPPALETTEENTRY);
    UINT     (*pGetTextCharsetInfo)(PHYSDEV,LPFONTSIGNATURE,DWORD);
    BOOL     (*pGetTextExtentExPoint)(PHYSDEV,LPCWSTR,INT,LPINT);
    BOOL     (*pGetTextExtentExPointI)(PHYSDEV,const WORD*,INT,LPINT);
    INT      (*pGetTextFace)(PHYSDEV,INT,LPWSTR);
    BOOL     (*pGetTextMetrics)(PHYSDEV,TEXTMETRICW*);
    BOOL     (*pGradientFill)(PHYSDEV,TRIVERTEX*,ULONG,void*,ULONG,ULONG);
    INT      (*pIntersectClipRect)(PHYSDEV,INT,INT,INT,INT);
    BOOL     (*pInvertRgn)(PHYSDEV,HRGN);
    BOOL     (*pLineTo)(PHYSDEV,INT,INT);
    BOOL     (*pModifyWorldTransform)(PHYSDEV,const XFORM*,DWORD);
    BOOL     (*pMoveTo)(PHYSDEV,INT,INT);
    INT      (*pOffsetClipRgn)(PHYSDEV,INT,INT);
    BOOL     (*pOffsetViewportOrgEx)(PHYSDEV,INT,INT,POINT*);
    BOOL     (*pOffsetWindowOrgEx)(PHYSDEV,INT,INT,POINT*);
    BOOL     (*pPaintRgn)(PHYSDEV,HRGN);
    BOOL     (*pPatBlt)(PHYSDEV,struct bitblt_coords*,DWORD);
    BOOL     (*pPie)(PHYSDEV,INT,INT,INT,INT,INT,INT,INT,INT);
    BOOL     (*pPolyBezier)(PHYSDEV,const POINT*,DWORD);
    BOOL     (*pPolyBezierTo)(PHYSDEV,const POINT*,DWORD);
    BOOL     (*pPolyDraw)(PHYSDEV,const POINT*,const BYTE *,DWORD);
    BOOL     (*pPolyPolygon)(PHYSDEV,const POINT*,const INT*,UINT);
    BOOL     (*pPolyPolyline)(PHYSDEV,const POINT*,const DWORD*,DWORD);
    BOOL     (*pPolygon)(PHYSDEV,const POINT*,INT);
    BOOL     (*pPolyline)(PHYSDEV,const POINT*,INT);
    BOOL     (*pPolylineTo)(PHYSDEV,const POINT*,INT);
    DWORD    (*pPutImage)(PHYSDEV,HRGN,BITMAPINFO*,const struct gdi_image_bits*,struct bitblt_coords*,struct bitblt_coords*,DWORD);
    UINT     (*pRealizeDefaultPalette)(PHYSDEV);
    UINT     (*pRealizePalette)(PHYSDEV,HPALETTE,BOOL);
    BOOL     (*pRectangle)(PHYSDEV,INT,INT,INT,INT);
    HDC      (*pResetDC)(PHYSDEV,const DEVMODEW*);
    BOOL     (*pRestoreDC)(PHYSDEV,INT);
    BOOL     (*pRoundRect)(PHYSDEV,INT,INT,INT,INT,INT,INT);
    INT      (*pSaveDC)(PHYSDEV);
    BOOL     (*pScaleViewportExtEx)(PHYSDEV,INT,INT,INT,INT,SIZE*);
    BOOL     (*pScaleWindowExtEx)(PHYSDEV,INT,INT,INT,INT,SIZE*);
    HBITMAP  (*pSelectBitmap)(PHYSDEV,HBITMAP);
    HBRUSH   (*pSelectBrush)(PHYSDEV,HBRUSH,const struct brush_pattern*);
    BOOL     (*pSelectClipPath)(PHYSDEV,INT);
    HFONT    (*pSelectFont)(PHYSDEV,HFONT,UINT*);
    HPALETTE (*pSelectPalette)(PHYSDEV,HPALETTE,BOOL);
    HPEN     (*pSelectPen)(PHYSDEV,HPEN,const struct brush_pattern*);
    INT      (*pSetArcDirection)(PHYSDEV,INT);
    COLORREF (*pSetBkColor)(PHYSDEV,COLORREF);
    INT      (*pSetBkMode)(PHYSDEV,INT);
    UINT     (*pSetBoundsRect)(PHYSDEV,RECT*,UINT);
    COLORREF (*pSetDCBrushColor)(PHYSDEV, COLORREF);
    COLORREF (*pSetDCPenColor)(PHYSDEV, COLORREF);
    INT      (*pSetDIBitsToDevice)(PHYSDEV,INT,INT,DWORD,DWORD,INT,INT,UINT,UINT,LPCVOID,BITMAPINFO*,UINT);
    VOID     (*pSetDeviceClipping)(PHYSDEV,HRGN);
    BOOL     (*pSetDeviceGammaRamp)(PHYSDEV,LPVOID);
    DWORD    (*pSetLayout)(PHYSDEV,DWORD);
    INT      (*pSetMapMode)(PHYSDEV,INT);
    DWORD    (*pSetMapperFlags)(PHYSDEV,DWORD);
    COLORREF (*pSetPixel)(PHYSDEV,INT,INT,COLORREF);
    INT      (*pSetPolyFillMode)(PHYSDEV,INT);
    INT      (*pSetROP2)(PHYSDEV,INT);
    INT      (*pSetRelAbs)(PHYSDEV,INT);
    INT      (*pSetStretchBltMode)(PHYSDEV,INT);
    UINT     (*pSetTextAlign)(PHYSDEV,UINT);
    INT      (*pSetTextCharacterExtra)(PHYSDEV,INT);
    COLORREF (*pSetTextColor)(PHYSDEV,COLORREF);
    BOOL     (*pSetTextJustification)(PHYSDEV,INT,INT);
    BOOL     (*pSetViewportExtEx)(PHYSDEV,INT,INT,SIZE*);
    BOOL     (*pSetViewportOrgEx)(PHYSDEV,INT,INT,POINT*);
    BOOL     (*pSetWindowExtEx)(PHYSDEV,INT,INT,SIZE*);
    BOOL     (*pSetWindowOrgEx)(PHYSDEV,INT,INT,POINT*);
    BOOL     (*pSetWorldTransform)(PHYSDEV,const XFORM*);
    INT      (*pStartDoc)(PHYSDEV,const DOCINFOW*);
    INT      (*pStartPage)(PHYSDEV);
    BOOL     (*pStretchBlt)(PHYSDEV,struct bitblt_coords*,PHYSDEV,struct bitblt_coords*,DWORD);
    INT      (*pStretchDIBits)(PHYSDEV,INT,INT,INT,INT,INT,INT,INT,INT,const void*,BITMAPINFO*,UINT,DWORD);
    BOOL     (*pStrokeAndFillPath)(PHYSDEV);
    BOOL     (*pStrokePath)(PHYSDEV);
    BOOL     (*pUnrealizePalette)(HPALETTE);
    BOOL     (*pWidenPath)(PHYSDEV);
    struct opengl_funcs * (*wine_get_wgl_driver)(PHYSDEV,UINT);

    /* priority order for the driver on the stack */
    UINT       priority;
};

typedef BOOL (CALLBACK *DCHOOKPROC)(HDC,WORD,DWORD_PTR,LPARAM);
typedef struct tagDC
{
    HDC          hSelf;            /* Handle to this DC */
    struct gdi_physdev nulldrv;    /* physdev for the null driver */
    PHYSDEV      physDev;          /* current top of the physdev stack */
    DWORD        thread;           /* thread owning the DC */
    LONG         refcount;         /* thread refcount */
    LONG         dirty;            /* dirty flag */
    INT          saveLevel;
    struct tagDC *saved_dc;
    DWORD_PTR    dwHookData;
    DCHOOKPROC   hookProc;         /* DC hook */
    BOOL         bounds_enabled:1; /* bounds tracking is enabled */
    BOOL         path_open:1;      /* path is currently open (only for saved DCs) */

    POINT        wnd_org;          /* Window origin */
    SIZE         wnd_ext;          /* Window extent */
    POINT        vport_org;        /* Viewport origin */
    SIZE         vport_ext;        /* Viewport extent */
    SIZE         virtual_res;      /* Initially HORZRES,VERTRES. Changed by SetVirtualResolution */
    SIZE         virtual_size;     /* Initially HORZSIZE,VERTSIZE. Changed by SetVirtualResolution */
    RECT         vis_rect;         /* visible rectangle in screen coords */
    RECT         device_rect;      /* rectangle for the whole device */
    int          pixel_format;     /* pixel format (for memory DCs) */
    UINT         aa_flags;         /* anti-aliasing flags to pass to GetGlyphOutline for current font */
    FLOAT        miterLimit;

    int           flags;
    DWORD         layout;
    HRGN          hClipRgn;      /* Clip region */
    HRGN          hMetaRgn;      /* Meta region */
    HRGN          hVisRgn;       /* Visible region */
    HRGN          region;        /* Total DC region (intersection of clip and visible) */
    HPEN          hPen;
    HBRUSH        hBrush;
    HFONT         hFont;
    HBITMAP       hBitmap;
    HPALETTE      hPalette;

    struct gdi_path *path;

    UINT          font_code_page;
    WORD          ROPmode;
    WORD          polyFillMode;
    WORD          stretchBltMode;
    WORD          relAbsMode;
    WORD          backgroundMode;
    COLORREF      backgroundColor;
    COLORREF      textColor;
    COLORREF      dcBrushColor;
    COLORREF      dcPenColor;
    POINT         brush_org;

    DWORD         mapperFlags;       /* Font mapper flags */
    WORD          textAlign;         /* Text alignment from SetTextAlign() */
    INT           charExtra;         /* Spacing from SetTextCharacterExtra() */
    INT           breakExtra;        /* breakTotalExtra / breakCount */
    INT           breakRem;          /* breakTotalExtra % breakCount */
    INT           MapMode;
    INT           GraphicsMode;      /* Graphics mode */
    ABORTPROC     pAbortProc;        /* AbortProc for Printing */
    POINT         cur_pos;           /* Current position */
    INT           ArcDirection;
    XFORM         xformWorld2Wnd;    /* World-to-window transformation */
    XFORM         xformWorld2Vport;  /* World-to-viewport transformation */
    XFORM         xformVport2World;  /* Inverse of the above transformation */
    BOOL          vport2WorldValid;  /* Is xformVport2World valid? */
    RECT          bounds;            /* Current bounding rect */
} DC;

/* Undocumented structure filled in by GetFontRealizationInfo */
struct font_realization_info
{
    DWORD size;        /* could be 16 or 24 */
    DWORD flags;       /* 1 for bitmap fonts, 3 for scalable fonts */
    DWORD cache_num;   /* keeps incrementing - num of fonts that have been created allowing for caching?? */
    DWORD instance_id; /* identifies a realized font instance */
    DWORD unk;         /* unknown */
    WORD  face_index;  /* face index in case of font collections */
    WORD  simulations; /* 0 bit - bold simulation, 1 bit - oblique simulation */
};

#define FIRST_GDI_HANDLE 32
#define MAX_GDI_HANDLES  16384

struct hdc_list
{
    HDC hdc;
    struct hdc_list *next;
};

struct gdi_handle_entry
{
    void                       *obj;         /* pointer to the object-specific data */
    const struct gdi_obj_funcs *funcs;       /* type-specific functions */
    struct hdc_list            *hdcs;        /* list of HDCs interested in this object */
    WORD                        generation;  /* generation count for reusing handle values */
    WORD                        type;        /* object type (one of the OBJ_* constants) */
    WORD                        selcount;    /* number of times the object is selected in a DC */
    WORD                        system : 1;  /* system object flag */
    WORD                        deleted : 1; /* whether DeleteObject has been called on this object */
};

static struct gdi_handle_entry gdi_handles[MAX_GDI_HANDLES];

static LONG debug_count;
HMODULE gdi32_module = 0;

static inline HGDIOBJ gentry_to_handle( struct gdi_handle_entry *entry )
{
    unsigned int idx = entry - gdi_handles + FIRST_GDI_HANDLE;
    return LongToHandle( idx | (entry->generation << 16) );
}

static inline struct gdi_handle_entry *ghandle_entry( HGDIOBJ handle )
{
    unsigned int idx = LOWORD(handle) - FIRST_GDI_HANDLE;

    if (idx < MAX_GDI_HANDLES && gdi_handles[idx].type)
    {
        if (!HIWORD( handle ) || HIWORD( handle ) == gdi_handles[idx].generation)
            return &gdi_handles[idx];
    }
    if (handle) WARN( "invalid handle %p\n", handle );
    return NULL;
}

static CRITICAL_SECTION gdi_section;
static CRITICAL_SECTION_DEBUG critsect_debug =
{
    0, 0, &gdi_section,
    { &critsect_debug.ProcessLocksList, &critsect_debug.ProcessLocksList },
      0, 0, { (DWORD_PTR)(__FILE__ ": gdi_section") }
};
static CRITICAL_SECTION gdi_section = { &critsect_debug, -1, 0, 0, 0, 0 };

/***********************************************************************
 *           GDI_ReleaseObj
 *
 */
void GDI_ReleaseObj( HGDIOBJ handle )
{
    LeaveCriticalSection( &gdi_section );
}

/***********************************************************************
 *           get_any_obj_ptr
 *
 * Return a pointer to, and the type of, the GDI object
 * associated with the handle.
 * The object must be released with GDI_ReleaseObj.
 */
void *get_any_obj_ptr( HGDIOBJ handle, WORD *type )
{
    void *ptr = NULL;
    struct gdi_handle_entry *entry;

    EnterCriticalSection( &gdi_section );

    if ((entry = ghandle_entry( handle )))
    {
        ptr = entry->obj;
        *type = entry->type;
    }

    if (!ptr) LeaveCriticalSection( &gdi_section );
    return ptr;
}

static inline DC *get_dc_obj( HDC hdc )
{
    WORD type;
    DC *dc = get_any_obj_ptr( hdc, &type );
    if (!dc) return NULL;

    switch (type)
    {
    case OBJ_DC:
    case OBJ_MEMDC:
    case OBJ_METADC:
    case OBJ_ENHMETADC:
        return dc;
    default:
        GDI_ReleaseObj( hdc );
        SetLastError( ERROR_INVALID_HANDLE );
        return NULL;
    }
}

/***********************************************************************
 *           get_dc_ptr
 *
 * Retrieve a DC pointer but release the GDI lock.
 */
DC *get_dc_ptr( HDC hdc )
{
    DC *dc = get_dc_obj( hdc );
    if (!dc) return NULL;

    if (!InterlockedCompareExchange( &dc->refcount, 1, 0 ))
    {
        dc->thread = GetCurrentThreadId();
    }
    else if (dc->thread != GetCurrentThreadId())
    {
        WARN( "dc %p belongs to thread %04x\n", hdc, dc->thread );
        GDI_ReleaseObj( hdc );
        return NULL;
    }
    else InterlockedIncrement( &dc->refcount );

    GDI_ReleaseObj( hdc );
    return dc;
}

void release_dc_ptr( DC *dc )
{
    LONG ref;

    dc->thread = 0;
    ref = InterlockedDecrement( &dc->refcount );
    assert( ref >= 0 );
    if (ref) dc->thread = GetCurrentThreadId();  /* we still own it */
}

static BOOL  (WINAPI *pGetFontRealizationInfo)(HDC hdc, DWORD *);
#define GET_DC_PHYSDEV(dc,func) \
    get_physdev_entry_point( (dc)->physDev, FIELD_OFFSET(struct gdi_dc_funcs,func))

static inline PHYSDEV get_physdev_entry_point( PHYSDEV dev, size_t offset )
{
    while (!((void **)dev->funcs)[offset / sizeof(void *)]) dev = dev->next;
    return dev;
}

/*************************************************************
 *           GetFontRealizationInfo    (GDI32.@)
 */
BOOL WINAPI GetFontRealizationInfo(HDC hdc, struct font_realization_info *info)
{
    BOOL is_v0 = info->size == FIELD_OFFSET(struct font_realization_info, unk);
    PHYSDEV dev;
    BOOL ret;
    DC *dc;

    if (info->size != sizeof(*info) && !is_v0)
        return FALSE;

    dc = get_dc_ptr(hdc);
    if (!dc) return FALSE;
    dev = GET_DC_PHYSDEV( dc, pGetFontRealizationInfo );
    ret = dev->funcs->pGetFontRealizationInfo( dev, info );
    release_dc_ptr(dc);
    return ret;
}

typedef struct tagGdiFont GdiFont;

struct list
{
    struct list *next;
    struct list *prev;
};

typedef struct {
    FT_Short height;
    FT_Short width;
    FT_Pos size;
    FT_Pos x_ppem;
    FT_Pos y_ppem;
    FT_Short internal_leading;
} Bitmap_Size;

typedef struct tagFace {
    struct list entry;
    unsigned int refcount;
    WCHAR *StyleName;
    WCHAR *FullName;
    WCHAR *file;
    dev_t dev;
    ino_t ino;
    void *font_data_ptr;
    DWORD font_data_size;
    FT_Long face_index;
    FONTSIGNATURE fs;
    DWORD ntmFlags;
    FT_Fixed font_version;
    BOOL scalable;
    Bitmap_Size size;     /* set if face is a bitmap */
    DWORD flags;          /* ADDFONT flags */
    struct tagFamily *family;
    /* Cached data for Enum */
    struct enum_data *cached_enum_data;
} Face;

typedef struct {
    GLYPHMETRICS gm;
    ABC          abc;  /* metrics of the unrotated char */
    BOOL         init;
} GM;

typedef struct {
    FLOAT eM11, eM12;
    FLOAT eM21, eM22;
} FMAT2;

typedef struct {
    DWORD hash;
    LOGFONTW lf;
    FMAT2 matrix;
    BOOL can_use_bitmap;
} FONT_DESC;

typedef struct {
    struct list entry;
    Face *face;
    GdiFont *font;
} CHILD_FONT;

struct font_fileinfo {
    FILETIME writetime;
    LARGE_INTEGER size;
    WCHAR path[1];
};

struct tagGdiFont {
    struct list entry;
    struct list unused_entry;
    unsigned int refcount;
    GM **gm;
    DWORD gmsize;
    OUTLINETEXTMETRICW *potm;
    DWORD total_kern_pairs;
    KERNINGPAIR *kern_pairs;
    struct list child_fonts;

    /* the following members can be accessed without locking, they are never modified after creation */
    FT_Face ft_face;
    struct font_mapping *mapping;
    LPWSTR name;
    int charset;
    int codepage;
    BOOL fake_italic;
    BOOL fake_bold;
    BYTE underline;
    BYTE strikeout;
    INT orientation;
    FONT_DESC font_desc;
    LONG aveWidth, ppem;
    double scale_y;
    SHORT yMax;
    SHORT yMin;
    DWORD ntmFlags;
    DWORD aa_flags;
    UINT ntmCellHeight, ntmAvgWidth;
    FONTSIGNATURE fs;
    GdiFont *base_font;
    VOID *GSUB_Table;
    const VOID *vert_feature;
    ULONG ttc_item_offset; /* 0 if font is not a part of TrueType collection */
    DWORD cache_num;
    DWORD instance_id;
    struct font_fileinfo *fileinfo;
};

typedef struct tagGdiFont GdiFont;

#define FIRST_FONT_HANDLE 1
#define MAX_FONT_HANDLES  256

struct font_handle_entry
{
    void *obj;
    WORD  generation; /* generation count for reusing handle values */
};

static struct font_handle_entry font_handles[MAX_FONT_HANDLES];


static inline DWORD fentry_to_handle( struct font_handle_entry *entry )
{
    unsigned int idx = entry - font_handles + FIRST_FONT_HANDLE;
    return idx | (entry->generation << 16);
}

static inline struct font_handle_entry *fhandle_entry( DWORD handle )
{
    unsigned int idx = LOWORD(handle) - FIRST_FONT_HANDLE;

    if (idx < MAX_FONT_HANDLES)
    {
        if (!HIWORD( handle ) || HIWORD( handle ) == font_handles[idx].generation)
            return &font_handles[idx];
    }
    if (handle) WARN( "invalid handle 0x%08x\n", handle );
    return NULL;
}
/*************************************************************************
 *             GetFontFileInfo   (GDI32.@)
 */
BOOL WINAPI GetFontFileInfo( DWORD instance_id, DWORD unknown, struct font_fileinfo *info, DWORD size, DWORD *needed )
{
    struct font_handle_entry *entry = fhandle_entry( instance_id );
    const GdiFont *font;

    if (!entry)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    font = entry->obj;
    *needed = sizeof(*info) + strlenW(font->fileinfo->path) * sizeof(WCHAR);
    if (*needed > size)
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }

    /* path is included too */
    memcpy(info, font->fileinfo, *needed);
    return TRUE;
}
