// Copyright 2021 Alex Brachet (alex@brachet.dev)
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CEDO_BINFMT_DWARFCONSTANTS_H
#define CEDO_BINFMT_DWARFCONSTANTS_H

#include <array>
#include <cstdint>

#include "DWARF.h"

enum class DWARFType {
  Zero = 0,
  One = 1,
  Two = 2,
  Four = 4,
  Eight = 8,

  DWARFAddr,   // Based on the AddressSize of the current section
  MachineAddr, // Based on the AddressSize of the ObjectFile
  String,
  StringPtr, // This is DWARFAddr
  LEB128,
  ULEB128,
  Indirect,
  Exprloc
};

// Generate in utils/DWARFConstants/GenConstants.py

struct DW_TAG {
  uint8_t value;
  constexpr operator decltype(value)() const { return value; }
};

constexpr DW_TAG DW_TAG_padding{0x00};
constexpr DW_TAG DW_TAG_array_type{0x01};
constexpr DW_TAG DW_TAG_class_type{0x02};
constexpr DW_TAG DW_TAG_entry_point{0x03};
constexpr DW_TAG DW_TAG_enumeration_type{0x04};
constexpr DW_TAG DW_TAG_formal_parameter{0x05};
constexpr DW_TAG DW_TAG_imported_declaration{0x08};
constexpr DW_TAG DW_TAG_label{0x0a};
constexpr DW_TAG DW_TAG_lexical_block{0x0b};
constexpr DW_TAG DW_TAG_member{0x0d};
constexpr DW_TAG DW_TAG_pointer_type{0x0f};
constexpr DW_TAG DW_TAG_reference_type{0x10};
constexpr DW_TAG DW_TAG_compile_unit{0x11};
constexpr DW_TAG DW_TAG_string_type{0x12};
constexpr DW_TAG DW_TAG_structure_type{0x13};
constexpr DW_TAG DW_TAG_subroutine_type{0x15};
constexpr DW_TAG DW_TAG_typedef{0x16};
constexpr DW_TAG DW_TAG_union_type{0x17};
constexpr DW_TAG DW_TAG_unspecified_parameters{0x18};
constexpr DW_TAG DW_TAG_variant{0x19};
constexpr DW_TAG DW_TAG_common_block{0x1a};
constexpr DW_TAG DW_TAG_common_inclusion{0x1b};
constexpr DW_TAG DW_TAG_inheritance{0x1c};
constexpr DW_TAG DW_TAG_inlined_subroutine{0x1d};
constexpr DW_TAG DW_TAG_module{0x1e};
constexpr DW_TAG DW_TAG_ptr_to_member_type{0x1f};
constexpr DW_TAG DW_TAG_set_type{0x20};
constexpr DW_TAG DW_TAG_subrange_type{0x21};
constexpr DW_TAG DW_TAG_with_stmt{0x22};
constexpr DW_TAG DW_TAG_access_declaration{0x23};
constexpr DW_TAG DW_TAG_base_type{0x24};
constexpr DW_TAG DW_TAG_catch_block{0x25};
constexpr DW_TAG DW_TAG_const_type{0x26};
constexpr DW_TAG DW_TAG_constant{0x27};
constexpr DW_TAG DW_TAG_enumerator{0x28};
constexpr DW_TAG DW_TAG_file_type{0x29};
constexpr DW_TAG DW_TAG_friend{0x2a};
constexpr DW_TAG DW_TAG_namelist{0x2b};
constexpr DW_TAG DW_TAG_namelist_item{0x2c};
constexpr DW_TAG DW_TAG_packed_type{0x2d};
constexpr DW_TAG DW_TAG_subprogram{0x2e};
constexpr DW_TAG DW_TAG_template_type_param{0x2f};
constexpr DW_TAG DW_TAG_template_value_param{0x30};
constexpr DW_TAG DW_TAG_thrown_type{0x31};
constexpr DW_TAG DW_TAG_try_block{0x32};
constexpr DW_TAG DW_TAG_variant_part{0x33};
constexpr DW_TAG DW_TAG_variable{0x34};
constexpr DW_TAG DW_TAG_volatile_type{0x35};
constexpr DW_TAG DW_TAG_dwarf_procedure{0x36};
constexpr DW_TAG DW_TAG_restrict_type{0x37};
constexpr DW_TAG DW_TAG_interface_type{0x38};
constexpr DW_TAG DW_TAG_namespace{0x39};
constexpr DW_TAG DW_TAG_imported_module{0x3a};
constexpr DW_TAG DW_TAG_unspecified_type{0x3b};
constexpr DW_TAG DW_TAG_partial_unit{0x3c};
constexpr DW_TAG DW_TAG_imported_unit{0x3d};
constexpr DW_TAG DW_TAG_condition{0x3f};
constexpr DW_TAG DW_TAG_shared_type{0x40};
constexpr DW_TAG DW_TAG_type_unit{0x41};
constexpr DW_TAG DW_TAG_rvalue_reference_type{0x42};
constexpr DW_TAG DW_TAG_template_alias{0x43};

struct DW_CHILDREN {
  uint8_t value;
  constexpr operator decltype(value)() const { return value; }
};

constexpr DW_CHILDREN DW_CHILDREN_no{0x00};
constexpr DW_CHILDREN DW_CHILDREN_yes{0x01};

struct DW_AT {
  uint8_t value;
  constexpr operator decltype(value)() const { return value; }
};

constexpr DW_AT DW_AT_sibling{0x01};
constexpr DW_AT DW_AT_location{0x02};
constexpr DW_AT DW_AT_name{0x03};
constexpr DW_AT DW_AT_ordering{0x09};
constexpr DW_AT DW_AT_subscr_data{0x0a};
constexpr DW_AT DW_AT_byte_size{0x0b};
constexpr DW_AT DW_AT_bit_offset{0x0c};
constexpr DW_AT DW_AT_bit_size{0x0d};
constexpr DW_AT DW_AT_element_list{0x0f};
constexpr DW_AT DW_AT_stmt_list{0x10};
constexpr DW_AT DW_AT_low_pc{0x11};
constexpr DW_AT DW_AT_high_pc{0x12};
constexpr DW_AT DW_AT_language{0x13};
constexpr DW_AT DW_AT_member{0x14};
constexpr DW_AT DW_AT_discr{0x15};
constexpr DW_AT DW_AT_discr_value{0x16};
constexpr DW_AT DW_AT_visibility{0x17};
constexpr DW_AT DW_AT_import{0x18};
constexpr DW_AT DW_AT_string_length{0x19};
constexpr DW_AT DW_AT_common_reference{0x1a};
constexpr DW_AT DW_AT_comp_dir{0x1b};
constexpr DW_AT DW_AT_const_value{0x1c};
constexpr DW_AT DW_AT_containing_type{0x1d};
constexpr DW_AT DW_AT_default_value{0x1e};
constexpr DW_AT DW_AT_inline{0x20};
constexpr DW_AT DW_AT_is_optional{0x21};
constexpr DW_AT DW_AT_lower_bound{0x22};
constexpr DW_AT DW_AT_producer{0x25};
constexpr DW_AT DW_AT_prototyped{0x27};
constexpr DW_AT DW_AT_return_addr{0x2a};
constexpr DW_AT DW_AT_start_scope{0x2c};
constexpr DW_AT DW_AT_bit_stride{0x2e};
constexpr DW_AT DW_AT_upper_bound{0x2f};
constexpr DW_AT DW_AT_abstract_origin{0x31};
constexpr DW_AT DW_AT_accessibility{0x32};
constexpr DW_AT DW_AT_address_class{0x33};
constexpr DW_AT DW_AT_artificial{0x34};
constexpr DW_AT DW_AT_base_types{0x35};
constexpr DW_AT DW_AT_calling_convention{0x36};
constexpr DW_AT DW_AT_count{0x37};
constexpr DW_AT DW_AT_data_member_location{0x38};
constexpr DW_AT DW_AT_decl_column{0x39};
constexpr DW_AT DW_AT_decl_file{0x3a};
constexpr DW_AT DW_AT_decl_line{0x3b};
constexpr DW_AT DW_AT_declaration{0x3c};
constexpr DW_AT DW_AT_discr_list{0x3d};
constexpr DW_AT DW_AT_encoding{0x3e};
constexpr DW_AT DW_AT_external{0x3f};
constexpr DW_AT DW_AT_frame_base{0x40};
constexpr DW_AT DW_AT_friend{0x41};
constexpr DW_AT DW_AT_identifier_case{0x42};
constexpr DW_AT DW_AT_macro_info{0x43};
constexpr DW_AT DW_AT_namelist_items{0x44};
constexpr DW_AT DW_AT_priority{0x45};
constexpr DW_AT DW_AT_segment{0x46};
constexpr DW_AT DW_AT_specification{0x47};
constexpr DW_AT DW_AT_static_link{0x48};
constexpr DW_AT DW_AT_type{0x49};
constexpr DW_AT DW_AT_use_location{0x4a};
constexpr DW_AT DW_AT_variable_parameter{0x4b};
constexpr DW_AT DW_AT_virtuality{0x4c};
constexpr DW_AT DW_AT_vtable_elem_location{0x4d};
constexpr DW_AT DW_AT_allocated{0x4e};
constexpr DW_AT DW_AT_associated{0x4f};
constexpr DW_AT DW_AT_data_location{0x50};
constexpr DW_AT DW_AT_byte_stride{0x51};
constexpr DW_AT DW_AT_entry_pc{0x52};
constexpr DW_AT DW_AT_use_UTF8{0x53};
constexpr DW_AT DW_AT_extension{0x54};
constexpr DW_AT DW_AT_ranges{0x55};
constexpr DW_AT DW_AT_trampoline{0x56};
constexpr DW_AT DW_AT_call_column{0x57};
constexpr DW_AT DW_AT_call_file{0x58};
constexpr DW_AT DW_AT_call_line{0x59};
constexpr DW_AT DW_AT_description{0x5a};
constexpr DW_AT DW_AT_binary_scale{0x5b};
constexpr DW_AT DW_AT_decimal_scale{0x5c};
constexpr DW_AT DW_AT_small{0x5d};
constexpr DW_AT DW_AT_decimal_sign{0x5e};
constexpr DW_AT DW_AT_digit_count{0x5f};
constexpr DW_AT DW_AT_picture_string{0x60};
constexpr DW_AT DW_AT_mutable{0x61};
constexpr DW_AT DW_AT_threads_scaled{0x62};
constexpr DW_AT DW_AT_explicit{0x63};
constexpr DW_AT DW_AT_object_pointer{0x64};
constexpr DW_AT DW_AT_endianity{0x65};
constexpr DW_AT DW_AT_elemental{0x66};
constexpr DW_AT DW_AT_pure{0x67};
constexpr DW_AT DW_AT_recursive{0x68};
constexpr DW_AT DW_AT_signature{0x69};
constexpr DW_AT DW_AT_main_subprogram{0x6a};
constexpr DW_AT DW_AT_data_bit_offset{0x6b};
constexpr DW_AT DW_AT_const_expr{0x6c};
constexpr DW_AT DW_AT_enum_class{0x6d};
constexpr DW_AT DW_AT_linkage_name{0x6e};

struct DW_FORM {
  uint8_t value;
  DWARFType type;
  constexpr operator decltype(value)() const { return value; }
};

constexpr std::array DW_FORM_static_list{
    DW_FORM{0x01, DWARFType::MachineAddr},
    DW_FORM{0x03, static_cast<DWARFType>(2)},
    DW_FORM{0x04, static_cast<DWARFType>(4)},
    DW_FORM{0x05, static_cast<DWARFType>(2)},
    DW_FORM{0x06, static_cast<DWARFType>(4)},
    DW_FORM{0x07, static_cast<DWARFType>(8)},
    DW_FORM{0x08, DWARFType::String},
    DW_FORM{0x09, DWARFType::LEB128},
    DW_FORM{0x0a, static_cast<DWARFType>(1)},
    DW_FORM{0x0b, static_cast<DWARFType>(1)},
    DW_FORM{0x0c, static_cast<DWARFType>(1)},
    DW_FORM{0x0d, DWARFType::LEB128},
    DW_FORM{0x0e, DWARFType::StringPtr},
    DW_FORM{0x0f, DWARFType::ULEB128},
    DW_FORM{0x10, DWARFType::DWARFAddr},
    DW_FORM{0x11, static_cast<DWARFType>(1)},
    DW_FORM{0x12, static_cast<DWARFType>(2)},
    DW_FORM{0x13, static_cast<DWARFType>(4)},
    DW_FORM{0x14, static_cast<DWARFType>(8)},
    DW_FORM{0x15, DWARFType::ULEB128},
    DW_FORM{0x16, DWARFType::Indirect},
    DW_FORM{0x17, DWARFType::DWARFAddr},
    DW_FORM{0x18, DWARFType::Exprloc},
    DW_FORM{0x19, static_cast<DWARFType>(0)},
    DW_FORM{0x20, static_cast<DWARFType>(8)}};
constexpr DW_FORM DW_FORM_form_addr = DW_FORM_static_list[0];
constexpr DW_FORM DW_FORM_block2 = DW_FORM_static_list[1];
constexpr DW_FORM DW_FORM_block4 = DW_FORM_static_list[2];
constexpr DW_FORM DW_FORM_data2 = DW_FORM_static_list[3];
constexpr DW_FORM DW_FORM_data4 = DW_FORM_static_list[4];
constexpr DW_FORM DW_FORM_data8 = DW_FORM_static_list[5];
constexpr DW_FORM DW_FORM_string = DW_FORM_static_list[6];
constexpr DW_FORM DW_FORM_block = DW_FORM_static_list[7];
constexpr DW_FORM DW_FORM_block1 = DW_FORM_static_list[8];
constexpr DW_FORM DW_FORM_data1 = DW_FORM_static_list[9];
constexpr DW_FORM DW_FORM_flag = DW_FORM_static_list[10];
constexpr DW_FORM DW_FORM_sdata = DW_FORM_static_list[11];
constexpr DW_FORM DW_FORM_strp = DW_FORM_static_list[12];
constexpr DW_FORM DW_FORM_udata = DW_FORM_static_list[13];
constexpr DW_FORM DW_FORM_ref_addr = DW_FORM_static_list[14];
constexpr DW_FORM DW_FORM_ref1 = DW_FORM_static_list[15];
constexpr DW_FORM DW_FORM_ref2 = DW_FORM_static_list[16];
constexpr DW_FORM DW_FORM_ref4 = DW_FORM_static_list[17];
constexpr DW_FORM DW_FORM_ref8 = DW_FORM_static_list[18];
constexpr DW_FORM DW_FORM_ref_udata = DW_FORM_static_list[19];
constexpr DW_FORM DW_FORM_indirect = DW_FORM_static_list[20];
constexpr DW_FORM DW_FORM_sec_offset = DW_FORM_static_list[21];
constexpr DW_FORM DW_FORM_exprloc = DW_FORM_static_list[22];
constexpr DW_FORM DW_FORM_flag_present = DW_FORM_static_list[23];
constexpr DW_FORM DW_FORM_ref_sig8 = DW_FORM_static_list[24];

constexpr DW_FORM get_DW_FORM(decltype(DW_FORM::value) value) {
  for (const auto &a : DW_FORM_static_list)
    if (a.value == value)
      return a;
  assert(0 && "value was not part of DW_FORM_static_list");
}

#endif // CEDO_BINFMT_DWARFCONSTANTS_H
