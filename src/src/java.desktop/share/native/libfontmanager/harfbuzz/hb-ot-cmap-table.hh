/*
 * Copyright © 2014  Google, Inc.
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Google Author(s): Behdad Esfahbod
 */

#ifndef HB_OT_CMAP_TABLE_HH
#define HB_OT_CMAP_TABLE_HH

#include "hb-open-type.hh"
#include "hb-set.hh"

/*
 * cmap -- Character to Glyph Index Mapping
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap
 */
#define HB_OT_TAG_cmap HB_TAG('c','m','a','p')

namespace OT {


struct CmapSubtableFormat0
{
  bool get_glyph (hb_codepoint_t codepoint, hb_codepoint_t *glyph) const
  {
    hb_codepoint_t gid = codepoint < 256 ? glyphIdArray[codepoint] : 0;
    if (!gid)
      return false;
    *glyph = gid;
    return true;
  }
  void collect_unicodes (hb_set_t *out) const
  {
    for (unsigned int i = 0; i < 256; i++)
      if (glyphIdArray[i])
        out->add (i);
  }

  bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (c->check_struct (this));
  }

  protected:
  HBUINT16      format;         /* Format number is set to 0. */
  HBUINT16      length;         /* Byte length of this subtable. */
  HBUINT16      language;       /* Ignore. */
  HBUINT8       glyphIdArray[256];/* An array that maps character
                                 * code to glyph index values. */
  public:
  DEFINE_SIZE_STATIC (6 + 256);
};

struct CmapSubtableFormat4
{
  struct segment_plan
  {
    HBUINT16 start_code;
    HBUINT16 end_code;
    bool use_delta;
  };

  bool serialize (hb_serialize_context_t *c,
                  const hb_subset_plan_t *plan,
                  const hb_vector_t<segment_plan> &segments)
  {
    TRACE_SERIALIZE (this);

    if (unlikely (!c->extend_min (*this))) return_trace (false);

    this->format.set (4);
    this->length.set (get_sub_table_size (segments));

    this->segCountX2.set (segments.length * 2);
    this->entrySelector.set (MAX (1u, hb_bit_storage (segments.length)) - 1);
    this->searchRange.set (2 * (1u << this->entrySelector));
    this->rangeShift.set (segments.length * 2 > this->searchRange
                          ? 2 * segments.length - this->searchRange
                          : 0);

    HBUINT16 *end_count = c->allocate_size<HBUINT16> (HBUINT16::static_size * segments.length);
    c->allocate_size<HBUINT16> (HBUINT16::static_size); // 2 bytes of padding.
    HBUINT16 *start_count = c->allocate_size<HBUINT16> (HBUINT16::static_size * segments.length);
    HBINT16 *id_delta = c->allocate_size<HBINT16> (HBUINT16::static_size * segments.length);
    HBUINT16 *id_range_offset = c->allocate_size<HBUINT16> (HBUINT16::static_size * segments.length);

    if (id_range_offset == nullptr)
      return_trace (false);

    for (unsigned int i = 0; i < segments.length; i++)
    {
      end_count[i].set (segments[i].end_code);
      start_count[i].set (segments[i].start_code);
      if (segments[i].use_delta)
      {
        hb_codepoint_t cp = segments[i].start_code;
        hb_codepoint_t start_gid = 0;
        if (unlikely (!plan->new_gid_for_codepoint (cp, &start_gid) && cp != 0xFFFF))
          return_trace (false);
        id_delta[i].set (start_gid - segments[i].start_code);
      } else {
        id_delta[i].set (0);
        unsigned int num_codepoints = segments[i].end_code - segments[i].start_code + 1;
        HBUINT16 *glyph_id_array = c->allocate_size<HBUINT16> (HBUINT16::static_size * num_codepoints);
        if (glyph_id_array == nullptr)
          return_trace (false);
        // From the cmap spec:
        //
        // id_range_offset[i]/2
        // + (cp - segments[i].start_code)
        // + (id_range_offset + i)
        // =
        // glyph_id_array + (cp - segments[i].start_code)
        //
        // So, solve for id_range_offset[i]:
        //
        // id_range_offset[i]
        // =
        // 2 * (glyph_id_array - id_range_offset - i)
        id_range_offset[i].set (2 * (
            glyph_id_array - id_range_offset - i));
        for (unsigned int j = 0; j < num_codepoints; j++)
        {
          hb_codepoint_t cp = segments[i].start_code + j;
          hb_codepoint_t new_gid;
          if (unlikely (!plan->new_gid_for_codepoint (cp, &new_gid)))
            return_trace (false);
          glyph_id_array[j].set (new_gid);
        }
      }
    }

    return_trace (true);
  }

  static size_t get_sub_table_size (const hb_vector_t<segment_plan> &segments)
  {
    size_t segment_size = 0;
    for (unsigned int i = 0; i < segments.length; i++)
    {
      // Parallel array entries
      segment_size +=
            2  // end count
          + 2  // start count
          + 2  // delta
          + 2; // range offset

      if (!segments[i].use_delta)
        // Add bytes for the glyph index array entries for this segment.
        segment_size += (segments[i].end_code - segments[i].start_code + 1) * 2;
    }

    return min_size
        + 2 // Padding
        + segment_size;
  }

  static bool create_sub_table_plan (const hb_subset_plan_t *plan,
                                     hb_vector_t<segment_plan> *segments)
  {
    segment_plan *segment = nullptr;
    hb_codepoint_t last_gid = 0;

    hb_codepoint_t cp = HB_SET_VALUE_INVALID;
    while (plan->unicodes->next (&cp)) {
      hb_codepoint_t new_gid;
      if (unlikely (!plan->new_gid_for_codepoint (cp, &new_gid)))
      {
        DEBUG_MSG(SUBSET, nullptr, "Unable to find new gid for %04x", cp);
        return false;
      }

      /* Stop adding to cmap if we are now outside of unicode BMP. */
      if (cp > 0xFFFF) break;

      if (!segment ||
          cp != segment->end_code + 1u)
      {
        segment = segments->push ();
        segment->start_code.set (cp);
        segment->end_code.set (cp);
        segment->use_delta = true;
      } else {
        segment->end_code.set (cp);
        if (last_gid + 1u != new_gid)
          // gid's are not consecutive in this segment so delta
          // cannot be used.
          segment->use_delta = false;
      }

      last_gid = new_gid;
    }

    // There must be a final entry with end_code == 0xFFFF. Check if we need to add one.
    if (segment == nullptr || segment->end_code != 0xFFFF)
    {
      segment = segments->push ();
      segment->start_code.set (0xFFFF);
      segment->end_code.set (0xFFFF);
      segment->use_delta = true;
    }

    return true;
  }

  struct accelerator_t
  {
    accelerator_t () {}
    accelerator_t (const CmapSubtableFormat4 *subtable) { init (subtable); }
    ~accelerator_t () { fini (); }

    void init (const CmapSubtableFormat4 *subtable)
    {
      segCount = subtable->segCountX2 / 2;
      endCount = subtable->values.arrayZ;
      startCount = endCount + segCount + 1;
      idDelta = startCount + segCount;
      idRangeOffset = idDelta + segCount;
      glyphIdArray = idRangeOffset + segCount;
      glyphIdArrayLength = (subtable->length - 16 - 8 * segCount) / 2;
    }
    void fini () {}

    bool get_glyph (hb_codepoint_t codepoint, hb_codepoint_t *glyph) const
    {
      /* Custom two-array bsearch. */
      int min = 0, max = (int) this->segCount - 1;
      const HBUINT16 *startCount = this->startCount;
      const HBUINT16 *endCount = this->endCount;
      unsigned int i;
      while (min <= max)
      {
        int mid = ((unsigned int) min + (unsigned int) max) / 2;
        if (codepoint < startCount[mid])
          max = mid - 1;
        else if (codepoint > endCount[mid])
          min = mid + 1;
        else
        {
          i = mid;
          goto found;
        }
      }
      return false;

    found:
      hb_codepoint_t gid;
      unsigned int rangeOffset = this->idRangeOffset[i];
      if (rangeOffset == 0)
        gid = codepoint + this->idDelta[i];
      else
      {
        /* Somebody has been smoking... */
        unsigned int index = rangeOffset / 2 + (codepoint - this->startCount[i]) + i - this->segCount;
        if (unlikely (index >= this->glyphIdArrayLength))
          return false;
        gid = this->glyphIdArray[index];
        if (unlikely (!gid))
          return false;
        gid += this->idDelta[i];
      }
      gid &= 0xFFFFu;
      if (!gid)
        return false;
      *glyph = gid;
      return true;
    }
    static bool get_glyph_func (const void *obj, hb_codepoint_t codepoint, hb_codepoint_t *glyph)
    {
      return ((const accelerator_t *) obj)->get_glyph (codepoint, glyph);
    }
    void collect_unicodes (hb_set_t *out) const
    {
      unsigned int count = this->segCount;
      if (count && this->startCount[count - 1] == 0xFFFFu)
        count--; /* Skip sentinel segment. */
      for (unsigned int i = 0; i < count; i++)
      {
        unsigned int rangeOffset = this->idRangeOffset[i];
        if (rangeOffset == 0)
          out->add_range (this->startCount[i], this->endCount[i]);
        else
        {
          for (hb_codepoint_t codepoint = this->startCount[i];
               codepoint <= this->endCount[i];
               codepoint++)
          {
            unsigned int index = rangeOffset / 2 + (codepoint - this->startCount[i]) + i - this->segCount;
            if (unlikely (index >= this->glyphIdArrayLength))
              break;
            hb_codepoint_t gid = this->glyphIdArray[index];
            if (unlikely (!gid))
              continue;
            out->add (codepoint);
          }
        }
      }
    }

    const HBUINT16 *endCount;
    const HBUINT16 *startCount;
    const HBUINT16 *idDelta;
    const HBUINT16 *idRangeOffset;
    const HBUINT16 *glyphIdArray;
    unsigned int segCount;
    unsigned int glyphIdArrayLength;
  };

  bool get_glyph (hb_codepoint_t codepoint, hb_codepoint_t *glyph) const
  {
    accelerator_t accel (this);
    return accel.get_glyph_func (&accel, codepoint, glyph);
  }
  void collect_unicodes (hb_set_t *out) const
  {
    accelerator_t accel (this);
    accel.collect_unicodes (out);
  }

  bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    if (unlikely (!c->check_struct (this)))
      return_trace (false);

    if (unlikely (!c->check_range (this, length)))
    {
      /* Some broken fonts have too long of a "length" value.
       * If that is the case, just change the value to truncate
       * the subtable at the end of the blob. */
      uint16_t new_length = (uint16_t) MIN ((uintptr_t) 65535,
                                            (uintptr_t) (c->end -
                                                         (char *) this));
      if (!c->try_set (&length, new_length))
        return_trace (false);
    }

    return_trace (16 + 4 * (unsigned int) segCountX2 <= length);
  }



  protected:
  HBUINT16      format;         /* Format number is set to 4. */
  HBUINT16      length;         /* This is the length in bytes of the
                                 * subtable. */
  HBUINT16      language;       /* Ignore. */
  HBUINT16      segCountX2;     /* 2 x segCount. */
  HBUINT16      searchRange;    /* 2 * (2**floor(log2(segCount))) */
  HBUINT16      entrySelector;  /* log2(searchRange/2) */
  HBUINT16      rangeShift;     /* 2 x segCount - searchRange */

  UnsizedArrayOf<HBUINT16>
                values;
#if 0
  HBUINT16      endCount[segCount];     /* End characterCode for each segment,
                                         * last=0xFFFFu. */
  HBUINT16      reservedPad;            /* Set to 0. */
  HBUINT16      startCount[segCount];   /* Start character code for each segment. */
  HBINT16               idDelta[segCount];      /* Delta for all character codes in segment. */
  HBUINT16      idRangeOffset[segCount];/* Offsets into glyphIdArray or 0 */
  UnsizedArrayOf<HBUINT16>
                glyphIdArray;   /* Glyph index array (arbitrary length) */
#endif

  public:
  DEFINE_SIZE_ARRAY (14, values);
};

struct CmapSubtableLongGroup
{
  friend struct CmapSubtableFormat12;
  friend struct CmapSubtableFormat13;
  template<typename U>
  friend struct CmapSubtableLongSegmented;
  friend struct cmap;

  int cmp (hb_codepoint_t codepoint) const
  {
    if (codepoint < startCharCode) return -1;
    if (codepoint > endCharCode)   return +1;
    return 0;
  }

  bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (c->check_struct (this));
  }

  private:
  HBUINT32              startCharCode;  /* First character code in this group. */
  HBUINT32              endCharCode;    /* Last character code in this group. */
  HBUINT32              glyphID;        /* Glyph index; interpretation depends on
                                         * subtable format. */
  public:
  DEFINE_SIZE_STATIC (12);
};
DECLARE_NULL_NAMESPACE_BYTES (OT, CmapSubtableLongGroup);

template <typename UINT>
struct CmapSubtableTrimmed
{
  bool get_glyph (hb_codepoint_t codepoint, hb_codepoint_t *glyph) const
  {
    /* Rely on our implicit array bound-checking. */
    hb_codepoint_t gid = glyphIdArray[codepoint - startCharCode];
    if (!gid)
      return false;
    *glyph = gid;
    return true;
  }
  void collect_unicodes (hb_set_t *out) const
  {
    hb_codepoint_t start = startCharCode;
    unsigned int count = glyphIdArray.len;
    for (unsigned int i = 0; i < count; i++)
      if (glyphIdArray[i])
        out->add (start + i);
  }

  bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (c->check_struct (this) && glyphIdArray.sanitize (c));
  }

  protected:
  UINT          formatReserved; /* Subtable format and (maybe) padding. */
  UINT          length;         /* Byte length of this subtable. */
  UINT          language;       /* Ignore. */
  UINT          startCharCode;  /* First character code covered. */
  ArrayOf<GlyphID, UINT>
                glyphIdArray;   /* Array of glyph index values for character
                                 * codes in the range. */
  public:
  DEFINE_SIZE_ARRAY (5 * sizeof (UINT), glyphIdArray);
};

struct CmapSubtableFormat6  : CmapSubtableTrimmed<HBUINT16> {};
struct CmapSubtableFormat10 : CmapSubtableTrimmed<HBUINT32 > {};

template <typename T>
struct CmapSubtableLongSegmented
{
  friend struct cmap;

  bool get_glyph (hb_codepoint_t codepoint, hb_codepoint_t *glyph) const
  {
    hb_codepoint_t gid = T::group_get_glyph (groups.bsearch (codepoint), codepoint);
    if (!gid)
      return false;
    *glyph = gid;
    return true;
  }

  void collect_unicodes (hb_set_t *out) const
  {
    for (unsigned int i = 0; i < this->groups.len; i++) {
      out->add_range (this->groups[i].startCharCode,
                      MIN ((hb_codepoint_t) this->groups[i].endCharCode,
                           (hb_codepoint_t) HB_UNICODE_MAX));
    }
  }

  bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (c->check_struct (this) && groups.sanitize (c));
  }

  bool serialize (hb_serialize_context_t *c,
                  const hb_vector_t<CmapSubtableLongGroup> &group_data)
  {
    TRACE_SERIALIZE (this);
    if (unlikely (!c->extend_min (*this))) return_trace (false);
    if (unlikely (!groups.serialize (c, group_data.as_array ()))) return_trace (false);
    return true;
  }

  protected:
  HBUINT16      format;         /* Subtable format; set to 12. */
  HBUINT16      reserved;       /* Reserved; set to 0. */
  HBUINT32      length;         /* Byte length of this subtable. */
  HBUINT32      language;       /* Ignore. */
  SortedArrayOf<CmapSubtableLongGroup, HBUINT32>
                groups;         /* Groupings. */
  public:
  DEFINE_SIZE_ARRAY (16, groups);
};

struct CmapSubtableFormat12 : CmapSubtableLongSegmented<CmapSubtableFormat12>
{
  static hb_codepoint_t group_get_glyph (const CmapSubtableLongGroup &group,
                                         hb_codepoint_t u)
  { return likely (group.startCharCode <= group.endCharCode) ?
           group.glyphID + (u - group.startCharCode) : 0; }


  bool serialize (hb_serialize_context_t *c,
                  const hb_vector_t<CmapSubtableLongGroup> &groups)
  {
    if (unlikely (!c->extend_min (*this))) return false;

    this->format.set (12);
    this->reserved.set (0);
    this->length.set (get_sub_table_size (groups));

    return CmapSubtableLongSegmented<CmapSubtableFormat12>::serialize (c, groups);
  }

  static size_t get_sub_table_size (const hb_vector_t<CmapSubtableLongGroup> &groups)
  {
    return 16 + 12 * groups.length;
  }

  static bool create_sub_table_plan (const hb_subset_plan_t *plan,
                                     hb_vector_t<CmapSubtableLongGroup> *groups)
  {
    CmapSubtableLongGroup *group = nullptr;

    hb_codepoint_t cp = HB_SET_VALUE_INVALID;
    while (plan->unicodes->next (&cp)) {
      hb_codepoint_t new_gid;
      if (unlikely (!plan->new_gid_for_codepoint (cp, &new_gid)))
      {
        DEBUG_MSG(SUBSET, nullptr, "Unable to find new gid for %04x", cp);
        return false;
      }

      if (!group || !_is_gid_consecutive (group, cp, new_gid))
      {
        group = groups->push ();
        group->startCharCode.set (cp);
        group->endCharCode.set (cp);
        group->glyphID.set (new_gid);
      }
      else group->endCharCode.set (cp);
    }

    DEBUG_MSG(SUBSET, nullptr, "cmap");
    for (unsigned int i = 0; i < groups->length; i++) {
      CmapSubtableLongGroup& group = (*groups)[i];
      DEBUG_MSG(SUBSET, nullptr, "  %d: U+%04X-U+%04X, gid %d-%d", i, (uint32_t) group.startCharCode, (uint32_t) group.endCharCode, (uint32_t) group.glyphID, (uint32_t) group.glyphID + ((uint32_t) group.endCharCode - (uint32_t) group.startCharCode));
    }

    return true;
  }

 private:
  static bool _is_gid_consecutive (CmapSubtableLongGroup *group,
                                   hb_codepoint_t cp,
                                   hb_codepoint_t new_gid)
  {
    return (cp - 1 == group->endCharCode) &&
        new_gid == group->glyphID + (cp - group->startCharCode);
  }

};

struct CmapSubtableFormat13 : CmapSubtableLongSegmented<CmapSubtableFormat13>
{
  static hb_codepoint_t group_get_glyph (const CmapSubtableLongGroup &group,
                                         hb_codepoint_t u HB_UNUSED)
  { return group.glyphID; }
};

typedef enum
{
  GLYPH_VARIANT_NOT_FOUND = 0,
  GLYPH_VARIANT_FOUND = 1,
  GLYPH_VARIANT_USE_DEFAULT = 2
} glyph_variant_t;

struct UnicodeValueRange
{
  int cmp (const hb_codepoint_t &codepoint) const
  {
    if (codepoint < startUnicodeValue) return -1;
    if (codepoint > startUnicodeValue + additionalCount) return +1;
    return 0;
  }

  bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (c->check_struct (this));
  }

  HBUINT24      startUnicodeValue;      /* First value in this range. */
  HBUINT8       additionalCount;        /* Number of additional values in this
                                         * range. */
  public:
  DEFINE_SIZE_STATIC (4);
};

struct DefaultUVS : SortedArrayOf<UnicodeValueRange, HBUINT32>
{
  void collect_unicodes (hb_set_t *out) const
  {
    unsigned int count = len;
    for (unsigned int i = 0; i < count; i++)
    {
      hb_codepoint_t first = arrayZ[i].startUnicodeValue;
      hb_codepoint_t last = MIN ((hb_codepoint_t) (first + arrayZ[i].additionalCount),
                                 (hb_codepoint_t) HB_UNICODE_MAX);
      out->add_range (first, last);
    }
  }

  public:
  DEFINE_SIZE_ARRAY (4, *this);
};

struct UVSMapping
{
  int cmp (const hb_codepoint_t &codepoint) const
  {
    return unicodeValue.cmp (codepoint);
  }

  bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (c->check_struct (this));
  }

  HBUINT24      unicodeValue;   /* Base Unicode value of the UVS */
  GlyphID       glyphID;        /* Glyph ID of the UVS */
  public:
  DEFINE_SIZE_STATIC (5);
};

struct NonDefaultUVS : SortedArrayOf<UVSMapping, HBUINT32>
{
  void collect_unicodes (hb_set_t *out) const
  {
    unsigned int count = len;
    for (unsigned int i = 0; i < count; i++)
      out->add (arrayZ[i].glyphID);
  }

  public:
  DEFINE_SIZE_ARRAY (4, *this);
};

struct VariationSelectorRecord
{
  glyph_variant_t get_glyph (hb_codepoint_t codepoint,
                             hb_codepoint_t *glyph,
                             const void *base) const
  {
    if ((base+defaultUVS).bfind (codepoint))
      return GLYPH_VARIANT_USE_DEFAULT;
    const UVSMapping &nonDefault = (base+nonDefaultUVS).bsearch (codepoint);
    if (nonDefault.glyphID)
    {
      *glyph = nonDefault.glyphID;
       return GLYPH_VARIANT_FOUND;
    }
    return GLYPH_VARIANT_NOT_FOUND;
  }

  void collect_unicodes (hb_set_t *out, const void *base) const
  {
    (base+defaultUVS).collect_unicodes (out);
    (base+nonDefaultUVS).collect_unicodes (out);
  }

  int cmp (const hb_codepoint_t &variation_selector) const
  {
    return varSelector.cmp (variation_selector);
  }

  bool sanitize (hb_sanitize_context_t *c, const void *base) const
  {
    TRACE_SANITIZE (this);
    return_trace (c->check_struct (this) &&
                  defaultUVS.sanitize (c, base) &&
                  nonDefaultUVS.sanitize (c, base));
  }

  HBUINT24      varSelector;    /* Variation selector. */
  LOffsetTo<DefaultUVS>
                defaultUVS;     /* Offset to Default UVS Table.  May be 0. */
  LOffsetTo<NonDefaultUVS>
                nonDefaultUVS;  /* Offset to Non-Default UVS Table.  May be 0. */
  public:
  DEFINE_SIZE_STATIC (11);
};

struct CmapSubtableFormat14
{
  glyph_variant_t get_glyph_variant (hb_codepoint_t codepoint,
                                     hb_codepoint_t variation_selector,
                                     hb_codepoint_t *glyph) const
  {
    return record.bsearch (variation_selector).get_glyph (codepoint, glyph, this);
  }

  void collect_variation_selectors (hb_set_t *out) const
  {
    unsigned int count = record.len;
    for (unsigned int i = 0; i < count; i++)
      out->add (record.arrayZ[i].varSelector);
  }
  void collect_variation_unicodes (hb_codepoint_t variation_selector,
                                   hb_set_t *out) const
  {
    record.bsearch (variation_selector).collect_unicodes (out, this);
  }

  bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (c->check_struct (this) &&
                  record.sanitize (c, this));
  }

  protected:
  HBUINT16      format;         /* Format number is set to 14. */
  HBUINT32      length;         /* Byte length of this subtable. */
  SortedArrayOf<VariationSelectorRecord, HBUINT32>
                record;         /* Variation selector records; sorted
                                 * in increasing order of `varSelector'. */
  public:
  DEFINE_SIZE_ARRAY (10, record);
};

struct CmapSubtable
{
  /* Note: We intentionally do NOT implement subtable formats 2 and 8. */

  bool get_glyph (hb_codepoint_t codepoint,
                  hb_codepoint_t *glyph) const
  {
    switch (u.format) {
    case  0: return u.format0 .get_glyph (codepoint, glyph);
    case  4: return u.format4 .get_glyph (codepoint, glyph);
    case  6: return u.format6 .get_glyph (codepoint, glyph);
    case 10: return u.format10.get_glyph (codepoint, glyph);
    case 12: return u.format12.get_glyph (codepoint, glyph);
    case 13: return u.format13.get_glyph (codepoint, glyph);
    case 14:
    default: return false;
    }
  }
  void collect_unicodes (hb_set_t *out) const
  {
    switch (u.format) {
    case  0: u.format0 .collect_unicodes (out); return;
    case  4: u.format4 .collect_unicodes (out); return;
    case  6: u.format6 .collect_unicodes (out); return;
    case 10: u.format10.collect_unicodes (out); return;
    case 12: u.format12.collect_unicodes (out); return;
    case 13: u.format13.collect_unicodes (out); return;
    case 14:
    default: return;
    }
  }

  bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    if (!u.format.sanitize (c)) return_trace (false);
    switch (u.format) {
    case  0: return_trace (u.format0 .sanitize (c));
    case  4: return_trace (u.format4 .sanitize (c));
    case  6: return_trace (u.format6 .sanitize (c));
    case 10: return_trace (u.format10.sanitize (c));
    case 12: return_trace (u.format12.sanitize (c));
    case 13: return_trace (u.format13.sanitize (c));
    case 14: return_trace (u.format14.sanitize (c));
    default:return_trace (true);
    }
  }

  public:
  union {
  HBUINT16              format;         /* Format identifier */
  CmapSubtableFormat0   format0;
  CmapSubtableFormat4   format4;
  CmapSubtableFormat6   format6;
  CmapSubtableFormat10  format10;
  CmapSubtableFormat12  format12;
  CmapSubtableFormat13  format13;
  CmapSubtableFormat14  format14;
  } u;
  public:
  DEFINE_SIZE_UNION (2, format);
};


struct EncodingRecord
{
  int cmp (const EncodingRecord &other) const
  {
    int ret;
    ret = platformID.cmp (other.platformID);
    if (ret) return ret;
    ret = encodingID.cmp (other.encodingID);
    if (ret) return ret;
    return 0;
  }

  bool sanitize (hb_sanitize_context_t *c, const void *base) const
  {
    TRACE_SANITIZE (this);
    return_trace (c->check_struct (this) &&
                  subtable.sanitize (c, base));
  }

  HBUINT16      platformID;     /* Platform ID. */
  HBUINT16      encodingID;     /* Platform-specific encoding ID. */
  LOffsetTo<CmapSubtable>
                subtable;       /* Byte offset from beginning of table to the subtable for this encoding. */
  public:
  DEFINE_SIZE_STATIC (8);
};

struct cmap
{
  static constexpr hb_tag_t tableTag = HB_OT_TAG_cmap;

  struct subset_plan
  {
    size_t final_size () const
    {
      return 4 // header
          +  8 * 3 // 3 EncodingRecord
          +  CmapSubtableFormat4::get_sub_table_size (this->format4_segments)
          +  CmapSubtableFormat12::get_sub_table_size (this->format12_groups);
    }

    hb_vector_t<CmapSubtableFormat4::segment_plan> format4_segments;
    hb_vector_t<CmapSubtableLongGroup> format12_groups;
  };

  bool _create_plan (const hb_subset_plan_t *plan,
                     subset_plan *cmap_plan) const
  {
    if (unlikely (!CmapSubtableFormat4::create_sub_table_plan (plan, &cmap_plan->format4_segments)))
      return false;

    return CmapSubtableFormat12::create_sub_table_plan (plan, &cmap_plan->format12_groups);
  }

  bool _subset (const hb_subset_plan_t *plan,
                const subset_plan &cmap_subset_plan,
                size_t dest_sz,
                void *dest) const
  {
    hb_serialize_context_t c (dest, dest_sz);

    cmap *table = c.start_serialize<cmap> ();
    if (unlikely (!c.extend_min (*table)))
    {
      return false;
    }

    table->version.set (0);

    if (unlikely (!table->encodingRecord.serialize (&c, /* numTables */ 3)))
      return false;

    // TODO(grieger): Convert the below to a for loop

    // Format 4, Plat 0 Encoding Record
    EncodingRecord &format4_plat0_rec = table->encodingRecord[0];
    format4_plat0_rec.platformID.set (0); // Unicode
    format4_plat0_rec.encodingID.set (3);

    // Format 4, Plat 3 Encoding Record
    EncodingRecord &format4_plat3_rec = table->encodingRecord[1];
    format4_plat3_rec.platformID.set (3); // Windows
    format4_plat3_rec.encodingID.set (1); // Unicode BMP

    // Format 12 Encoding Record
    EncodingRecord &format12_rec = table->encodingRecord[2];
    format12_rec.platformID.set (3); // Windows
    format12_rec.encodingID.set (10); // Unicode UCS-4

    // Write out format 4 sub table
    {
      CmapSubtable &subtable = format4_plat0_rec.subtable.serialize (&c, table);
      format4_plat3_rec.subtable.set (format4_plat0_rec.subtable);
      subtable.u.format.set (4);

      CmapSubtableFormat4 &format4 = subtable.u.format4;
      if (unlikely (!format4.serialize (&c, plan, cmap_subset_plan.format4_segments)))
        return false;
    }

    // Write out format 12 sub table.
    {
      CmapSubtable &subtable = format12_rec.subtable.serialize (&c, table);
      subtable.u.format.set (12);

      CmapSubtableFormat12 &format12 = subtable.u.format12;
      if (unlikely (!format12.serialize (&c, cmap_subset_plan.format12_groups)))
        return false;
    }

    c.end_serialize ();

    return true;
  }

  bool subset (hb_subset_plan_t *plan) const
  {
    subset_plan cmap_subset_plan;

    if (unlikely (!_create_plan (plan, &cmap_subset_plan)))
    {
      DEBUG_MSG(SUBSET, nullptr, "Failed to generate a cmap subsetting plan.");
      return false;
    }

    // We now know how big our blob needs to be
    size_t dest_sz = cmap_subset_plan.final_size ();
    void *dest = malloc (dest_sz);
    if (unlikely (!dest)) {
      DEBUG_MSG(SUBSET, nullptr, "Unable to alloc %lu for cmap subset output", (unsigned long) dest_sz);
      return false;
    }

    if (unlikely (!_subset (plan, cmap_subset_plan, dest_sz, dest)))
    {
      DEBUG_MSG(SUBSET, nullptr, "Failed to perform subsetting of cmap.");
      free (dest);
      return false;
    }

    // all done, write the blob into dest
    hb_blob_t *cmap_prime = hb_blob_create ((const char *) dest,
                                            dest_sz,
                                            HB_MEMORY_MODE_READONLY,
                                            dest,
                                            free);
    bool result =  plan->add_table (HB_OT_TAG_cmap, cmap_prime);
    hb_blob_destroy (cmap_prime);
    return result;
  }

  const CmapSubtable *find_best_subtable (bool *symbol = nullptr) const
  {
    if (symbol) *symbol = false;

    const CmapSubtable *subtable;

    /* 32-bit subtables. */
    if ((subtable = this->find_subtable (3, 10))) return subtable;
    if ((subtable = this->find_subtable (0, 6))) return subtable;
    if ((subtable = this->find_subtable (0, 4))) return subtable;

    /* 16-bit subtables. */
    if ((subtable = this->find_subtable (3, 1))) return subtable;
    if ((subtable = this->find_subtable (0, 3))) return subtable;
    if ((subtable = this->find_subtable (0, 2))) return subtable;
    if ((subtable = this->find_subtable (0, 1))) return subtable;
    if ((subtable = this->find_subtable (0, 0))) return subtable;

    /* Symbol subtable. */
    if ((subtable = this->find_subtable (3, 0)))
    {
      if (symbol) *symbol = true;
      return subtable;
    }

    /* Meh. */
    return &Null (CmapSubtable);
  }

  struct accelerator_t
  {
    void init (hb_face_t *face)
    {
      this->table = hb_sanitize_context_t ().reference_table<cmap> (face);
      bool symbol;
      this->subtable = table->find_best_subtable (&symbol);
      this->subtable_uvs = &Null (CmapSubtableFormat14);
      {
        const CmapSubtable *st = table->find_subtable (0, 5);
        if (st && st->u.format == 14)
          subtable_uvs = &st->u.format14;
      }

      this->get_glyph_data = subtable;
      if (unlikely (symbol))
      {
        this->get_glyph_funcZ = get_glyph_from_symbol<CmapSubtable>;
      } else {
        switch (subtable->u.format) {
        /* Accelerate format 4 and format 12. */
        default:
          this->get_glyph_funcZ = get_glyph_from<CmapSubtable>;
          break;
        case 12:
          this->get_glyph_funcZ = get_glyph_from<CmapSubtableFormat12>;
          break;
        case  4:
          {
            this->format4_accel.init (&subtable->u.format4);
            this->get_glyph_data = &this->format4_accel;
            this->get_glyph_funcZ = this->format4_accel.get_glyph_func;
          }
          break;
        }
      }
    }

    void fini () { this->table.destroy (); }

    bool get_nominal_glyph (hb_codepoint_t  unicode,
                                   hb_codepoint_t *glyph) const
    {
      if (unlikely (!this->get_glyph_funcZ)) return false;
      return this->get_glyph_funcZ (this->get_glyph_data, unicode, glyph);
    }
    unsigned int get_nominal_glyphs (unsigned int count,
                                     const hb_codepoint_t *first_unicode,
                                     unsigned int unicode_stride,
                                     hb_codepoint_t *first_glyph,
                                     unsigned int glyph_stride) const
    {
      if (unlikely (!this->get_glyph_funcZ)) return 0;

      hb_cmap_get_glyph_func_t get_glyph_funcZ = this->get_glyph_funcZ;
      const void *get_glyph_data = this->get_glyph_data;

      unsigned int done;
      for (done = 0;
           done < count && get_glyph_funcZ (get_glyph_data, *first_unicode, first_glyph);
           done++)
      {
        first_unicode = &StructAtOffsetUnaligned<hb_codepoint_t> (first_unicode, unicode_stride);
        first_glyph = &StructAtOffsetUnaligned<hb_codepoint_t> (first_glyph, glyph_stride);
      }
      return done;
    }

    bool get_variation_glyph (hb_codepoint_t  unicode,
                              hb_codepoint_t  variation_selector,
                              hb_codepoint_t *glyph) const
    {
      switch (this->subtable_uvs->get_glyph_variant (unicode,
                                                     variation_selector,
                                                     glyph))
      {
        case GLYPH_VARIANT_NOT_FOUND:   return false;
        case GLYPH_VARIANT_FOUND:       return true;
        case GLYPH_VARIANT_USE_DEFAULT: break;
      }

      return get_nominal_glyph (unicode, glyph);
    }

    void collect_unicodes (hb_set_t *out) const
    {
      subtable->collect_unicodes (out);
    }
    void collect_variation_selectors (hb_set_t *out) const
    {
      subtable_uvs->collect_variation_selectors (out);
    }
    void collect_variation_unicodes (hb_codepoint_t variation_selector,
                                     hb_set_t *out) const
    {
      subtable_uvs->collect_variation_unicodes (variation_selector, out);
    }

    protected:
    typedef bool (*hb_cmap_get_glyph_func_t) (const void *obj,
                                              hb_codepoint_t codepoint,
                                              hb_codepoint_t *glyph);

    template <typename Type>
    static bool get_glyph_from (const void *obj,
                                hb_codepoint_t codepoint,
                                hb_codepoint_t *glyph)
    {
      const Type *typed_obj = (const Type *) obj;
      return typed_obj->get_glyph (codepoint, glyph);
    }

    template <typename Type>
    static bool get_glyph_from_symbol (const void *obj,
                                              hb_codepoint_t codepoint,
                                              hb_codepoint_t *glyph)
    {
      const Type *typed_obj = (const Type *) obj;
      if (likely (typed_obj->get_glyph (codepoint, glyph)))
        return true;

      if (codepoint <= 0x00FFu)
      {
        /* For symbol-encoded OpenType fonts, we duplicate the
         * U+F000..F0FF range at U+0000..U+00FF.  That's what
         * Windows seems to do, and that's hinted about at:
         * https://docs.microsoft.com/en-us/typography/opentype/spec/recom
         * under "Non-Standard (Symbol) Fonts". */
        return typed_obj->get_glyph (0xF000u + codepoint, glyph);
      }

      return false;
    }

    private:
    hb_nonnull_ptr_t<const CmapSubtable> subtable;
    hb_nonnull_ptr_t<const CmapSubtableFormat14> subtable_uvs;

    hb_cmap_get_glyph_func_t get_glyph_funcZ;
    const void *get_glyph_data;

    CmapSubtableFormat4::accelerator_t format4_accel;

    hb_blob_ptr_t<cmap> table;
  };

  protected:

  const CmapSubtable *find_subtable (unsigned int platform_id,
                                     unsigned int encoding_id) const
  {
    EncodingRecord key;
    key.platformID.set (platform_id);
    key.encodingID.set (encoding_id);

    const EncodingRecord &result = encodingRecord.bsearch (key);
    if (!result.subtable)
      return nullptr;

    return &(this+result.subtable);
  }

  public:

  bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (c->check_struct (this) &&
                  likely (version == 0) &&
                  encodingRecord.sanitize (c, this));
  }

  protected:
  HBUINT16              version;        /* Table version number (0). */
  SortedArrayOf<EncodingRecord>
                        encodingRecord; /* Encoding tables. */
  public:
  DEFINE_SIZE_ARRAY (4, encodingRecord);
};

struct cmap_accelerator_t : cmap::accelerator_t {};

} /* namespace OT */


#endif /* HB_OT_CMAP_TABLE_HH */
