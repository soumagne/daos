// Code generated by protoc-gen-go. DO NOT EDIT.
// source: storage.proto

package ctl

import (
	fmt "fmt"
	proto "github.com/golang/protobuf/proto"
	math "math"
)

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion3 // please upgrade the proto package

type StoragePrepareReq struct {
	Nvme                 *PrepareNvmeReq `protobuf:"bytes,1,opt,name=nvme,proto3" json:"nvme,omitempty"`
	Scm                  *PrepareScmReq  `protobuf:"bytes,2,opt,name=scm,proto3" json:"scm,omitempty"`
	XXX_NoUnkeyedLiteral struct{}        `json:"-"`
	XXX_unrecognized     []byte          `json:"-"`
	XXX_sizecache        int32           `json:"-"`
}

func (m *StoragePrepareReq) Reset()         { *m = StoragePrepareReq{} }
func (m *StoragePrepareReq) String() string { return proto.CompactTextString(m) }
func (*StoragePrepareReq) ProtoMessage()    {}
func (*StoragePrepareReq) Descriptor() ([]byte, []int) {
	return fileDescriptor_0d2c4ccf1453ffdb, []int{0}
}

func (m *StoragePrepareReq) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StoragePrepareReq.Unmarshal(m, b)
}
func (m *StoragePrepareReq) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StoragePrepareReq.Marshal(b, m, deterministic)
}
func (m *StoragePrepareReq) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StoragePrepareReq.Merge(m, src)
}
func (m *StoragePrepareReq) XXX_Size() int {
	return xxx_messageInfo_StoragePrepareReq.Size(m)
}
func (m *StoragePrepareReq) XXX_DiscardUnknown() {
	xxx_messageInfo_StoragePrepareReq.DiscardUnknown(m)
}

var xxx_messageInfo_StoragePrepareReq proto.InternalMessageInfo

func (m *StoragePrepareReq) GetNvme() *PrepareNvmeReq {
	if m != nil {
		return m.Nvme
	}
	return nil
}

func (m *StoragePrepareReq) GetScm() *PrepareScmReq {
	if m != nil {
		return m.Scm
	}
	return nil
}

type StoragePrepareResp struct {
	Nvme                 *PrepareNvmeResp `protobuf:"bytes,1,opt,name=nvme,proto3" json:"nvme,omitempty"`
	Scm                  *PrepareScmResp  `protobuf:"bytes,2,opt,name=scm,proto3" json:"scm,omitempty"`
	XXX_NoUnkeyedLiteral struct{}         `json:"-"`
	XXX_unrecognized     []byte           `json:"-"`
	XXX_sizecache        int32            `json:"-"`
}

func (m *StoragePrepareResp) Reset()         { *m = StoragePrepareResp{} }
func (m *StoragePrepareResp) String() string { return proto.CompactTextString(m) }
func (*StoragePrepareResp) ProtoMessage()    {}
func (*StoragePrepareResp) Descriptor() ([]byte, []int) {
	return fileDescriptor_0d2c4ccf1453ffdb, []int{1}
}

func (m *StoragePrepareResp) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StoragePrepareResp.Unmarshal(m, b)
}
func (m *StoragePrepareResp) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StoragePrepareResp.Marshal(b, m, deterministic)
}
func (m *StoragePrepareResp) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StoragePrepareResp.Merge(m, src)
}
func (m *StoragePrepareResp) XXX_Size() int {
	return xxx_messageInfo_StoragePrepareResp.Size(m)
}
func (m *StoragePrepareResp) XXX_DiscardUnknown() {
	xxx_messageInfo_StoragePrepareResp.DiscardUnknown(m)
}

var xxx_messageInfo_StoragePrepareResp proto.InternalMessageInfo

func (m *StoragePrepareResp) GetNvme() *PrepareNvmeResp {
	if m != nil {
		return m.Nvme
	}
	return nil
}

func (m *StoragePrepareResp) GetScm() *PrepareScmResp {
	if m != nil {
		return m.Scm
	}
	return nil
}

type StorageScanReq struct {
	Nvme                 *ScanNvmeReq `protobuf:"bytes,1,opt,name=nvme,proto3" json:"nvme,omitempty"`
	Scm                  *ScanScmReq  `protobuf:"bytes,2,opt,name=scm,proto3" json:"scm,omitempty"`
	XXX_NoUnkeyedLiteral struct{}     `json:"-"`
	XXX_unrecognized     []byte       `json:"-"`
	XXX_sizecache        int32        `json:"-"`
}

func (m *StorageScanReq) Reset()         { *m = StorageScanReq{} }
func (m *StorageScanReq) String() string { return proto.CompactTextString(m) }
func (*StorageScanReq) ProtoMessage()    {}
func (*StorageScanReq) Descriptor() ([]byte, []int) {
	return fileDescriptor_0d2c4ccf1453ffdb, []int{2}
}

func (m *StorageScanReq) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StorageScanReq.Unmarshal(m, b)
}
func (m *StorageScanReq) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StorageScanReq.Marshal(b, m, deterministic)
}
func (m *StorageScanReq) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StorageScanReq.Merge(m, src)
}
func (m *StorageScanReq) XXX_Size() int {
	return xxx_messageInfo_StorageScanReq.Size(m)
}
func (m *StorageScanReq) XXX_DiscardUnknown() {
	xxx_messageInfo_StorageScanReq.DiscardUnknown(m)
}

var xxx_messageInfo_StorageScanReq proto.InternalMessageInfo

func (m *StorageScanReq) GetNvme() *ScanNvmeReq {
	if m != nil {
		return m.Nvme
	}
	return nil
}

func (m *StorageScanReq) GetScm() *ScanScmReq {
	if m != nil {
		return m.Scm
	}
	return nil
}

type StorageScanResp struct {
	Nvme                 *ScanNvmeResp `protobuf:"bytes,1,opt,name=nvme,proto3" json:"nvme,omitempty"`
	Scm                  *ScanScmResp  `protobuf:"bytes,2,opt,name=scm,proto3" json:"scm,omitempty"`
	XXX_NoUnkeyedLiteral struct{}      `json:"-"`
	XXX_unrecognized     []byte        `json:"-"`
	XXX_sizecache        int32         `json:"-"`
}

func (m *StorageScanResp) Reset()         { *m = StorageScanResp{} }
func (m *StorageScanResp) String() string { return proto.CompactTextString(m) }
func (*StorageScanResp) ProtoMessage()    {}
func (*StorageScanResp) Descriptor() ([]byte, []int) {
	return fileDescriptor_0d2c4ccf1453ffdb, []int{3}
}

func (m *StorageScanResp) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StorageScanResp.Unmarshal(m, b)
}
func (m *StorageScanResp) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StorageScanResp.Marshal(b, m, deterministic)
}
func (m *StorageScanResp) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StorageScanResp.Merge(m, src)
}
func (m *StorageScanResp) XXX_Size() int {
	return xxx_messageInfo_StorageScanResp.Size(m)
}
func (m *StorageScanResp) XXX_DiscardUnknown() {
	xxx_messageInfo_StorageScanResp.DiscardUnknown(m)
}

var xxx_messageInfo_StorageScanResp proto.InternalMessageInfo

func (m *StorageScanResp) GetNvme() *ScanNvmeResp {
	if m != nil {
		return m.Nvme
	}
	return nil
}

func (m *StorageScanResp) GetScm() *ScanScmResp {
	if m != nil {
		return m.Scm
	}
	return nil
}

type StorageFormatReq struct {
	Nvme                 *FormatNvmeReq `protobuf:"bytes,1,opt,name=nvme,proto3" json:"nvme,omitempty"`
	Scm                  *FormatScmReq  `protobuf:"bytes,2,opt,name=scm,proto3" json:"scm,omitempty"`
	Reformat             bool           `protobuf:"varint,3,opt,name=reformat,proto3" json:"reformat,omitempty"`
	XXX_NoUnkeyedLiteral struct{}       `json:"-"`
	XXX_unrecognized     []byte         `json:"-"`
	XXX_sizecache        int32          `json:"-"`
}

func (m *StorageFormatReq) Reset()         { *m = StorageFormatReq{} }
func (m *StorageFormatReq) String() string { return proto.CompactTextString(m) }
func (*StorageFormatReq) ProtoMessage()    {}
func (*StorageFormatReq) Descriptor() ([]byte, []int) {
	return fileDescriptor_0d2c4ccf1453ffdb, []int{4}
}

func (m *StorageFormatReq) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StorageFormatReq.Unmarshal(m, b)
}
func (m *StorageFormatReq) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StorageFormatReq.Marshal(b, m, deterministic)
}
func (m *StorageFormatReq) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StorageFormatReq.Merge(m, src)
}
func (m *StorageFormatReq) XXX_Size() int {
	return xxx_messageInfo_StorageFormatReq.Size(m)
}
func (m *StorageFormatReq) XXX_DiscardUnknown() {
	xxx_messageInfo_StorageFormatReq.DiscardUnknown(m)
}

var xxx_messageInfo_StorageFormatReq proto.InternalMessageInfo

func (m *StorageFormatReq) GetNvme() *FormatNvmeReq {
	if m != nil {
		return m.Nvme
	}
	return nil
}

func (m *StorageFormatReq) GetScm() *FormatScmReq {
	if m != nil {
		return m.Scm
	}
	return nil
}

func (m *StorageFormatReq) GetReformat() bool {
	if m != nil {
		return m.Reformat
	}
	return false
}

type StorageFormatResp struct {
	Crets                []*NvmeControllerResult `protobuf:"bytes,1,rep,name=crets,proto3" json:"crets,omitempty"`
	Mrets                []*ScmMountResult       `protobuf:"bytes,2,rep,name=mrets,proto3" json:"mrets,omitempty"`
	XXX_NoUnkeyedLiteral struct{}                `json:"-"`
	XXX_unrecognized     []byte                  `json:"-"`
	XXX_sizecache        int32                   `json:"-"`
}

func (m *StorageFormatResp) Reset()         { *m = StorageFormatResp{} }
func (m *StorageFormatResp) String() string { return proto.CompactTextString(m) }
func (*StorageFormatResp) ProtoMessage()    {}
func (*StorageFormatResp) Descriptor() ([]byte, []int) {
	return fileDescriptor_0d2c4ccf1453ffdb, []int{5}
}

func (m *StorageFormatResp) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_StorageFormatResp.Unmarshal(m, b)
}
func (m *StorageFormatResp) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_StorageFormatResp.Marshal(b, m, deterministic)
}
func (m *StorageFormatResp) XXX_Merge(src proto.Message) {
	xxx_messageInfo_StorageFormatResp.Merge(m, src)
}
func (m *StorageFormatResp) XXX_Size() int {
	return xxx_messageInfo_StorageFormatResp.Size(m)
}
func (m *StorageFormatResp) XXX_DiscardUnknown() {
	xxx_messageInfo_StorageFormatResp.DiscardUnknown(m)
}

var xxx_messageInfo_StorageFormatResp proto.InternalMessageInfo

func (m *StorageFormatResp) GetCrets() []*NvmeControllerResult {
	if m != nil {
		return m.Crets
	}
	return nil
}

func (m *StorageFormatResp) GetMrets() []*ScmMountResult {
	if m != nil {
		return m.Mrets
	}
	return nil
}

func init() {
	proto.RegisterType((*StoragePrepareReq)(nil), "ctl.StoragePrepareReq")
	proto.RegisterType((*StoragePrepareResp)(nil), "ctl.StoragePrepareResp")
	proto.RegisterType((*StorageScanReq)(nil), "ctl.StorageScanReq")
	proto.RegisterType((*StorageScanResp)(nil), "ctl.StorageScanResp")
	proto.RegisterType((*StorageFormatReq)(nil), "ctl.StorageFormatReq")
	proto.RegisterType((*StorageFormatResp)(nil), "ctl.StorageFormatResp")
}

func init() {
	proto.RegisterFile("storage.proto", fileDescriptor_0d2c4ccf1453ffdb)
}

var fileDescriptor_0d2c4ccf1453ffdb = []byte{
	// 320 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x64, 0x92, 0xbf, 0x4e, 0xc3, 0x30,
	0x10, 0x87, 0x95, 0x86, 0xa2, 0xca, 0x15, 0xb4, 0xb9, 0x32, 0x84, 0x4c, 0x21, 0xb4, 0x10, 0x96,
	0x20, 0x95, 0x47, 0x40, 0x62, 0x03, 0xa1, 0x64, 0x42, 0x42, 0x42, 0xa9, 0x65, 0x58, 0xe2, 0xd8,
	0xd8, 0x6e, 0x17, 0x5e, 0x1e, 0xf9, 0x4f, 0x22, 0xbb, 0x1d, 0x73, 0xf7, 0xbb, 0xef, 0xbb, 0x8b,
	0x8c, 0x2e, 0xa4, 0x62, 0xa2, 0xfd, 0x21, 0x15, 0x17, 0x4c, 0x31, 0x88, 0xb1, 0xea, 0x32, 0x70,
	0xb5, 0xaf, 0xfe, 0x40, 0x5d, 0x23, 0x4b, 0x86, 0x9a, 0xc4, 0xd4, 0x96, 0x8a, 0x1d, 0x4a, 0x1a,
	0x5b, 0x7c, 0x17, 0x84, 0xb7, 0x82, 0xd4, 0xe4, 0x17, 0xee, 0xd1, 0x99, 0x9e, 0x4a, 0xa3, 0x3c,
	0x2a, 0xe7, 0xdb, 0x55, 0x85, 0x55, 0x57, 0xb9, 0xf6, 0xdb, 0x81, 0xea, 0x48, 0x6d, 0x02, 0xb0,
	0x46, 0xb1, 0xc4, 0x34, 0x9d, 0x98, 0x1c, 0xf8, 0xb9, 0x06, 0x53, 0x1d, 0xd3, 0xed, 0x82, 0x20,
	0x38, 0x76, 0x48, 0x0e, 0x65, 0x20, 0xb9, 0x3a, 0x95, 0x48, 0xee, 0x2c, 0x1b, 0xdf, 0xb2, 0x3a,
	0xb1, 0x48, 0x6e, 0x35, 0x1f, 0xe8, 0xd2, 0x69, 0x1a, 0xdc, 0xf6, 0xfa, 0x8e, 0x75, 0xa0, 0x58,
	0x9a, 0x49, 0xdd, 0x0b, 0x8f, 0xb8, 0xf1, 0xf1, 0x8b, 0x31, 0xe4, 0x5f, 0xf0, 0x89, 0x16, 0x01,
	0x5a, 0x72, 0xd8, 0x04, 0xec, 0xe4, 0x88, 0x3d, 0xee, 0x5e, 0xf8, 0xf0, 0x65, 0x08, 0x1f, 0x16,
	0xff, 0x43, 0x4b, 0x47, 0x7f, 0x61, 0x82, 0xb6, 0x4a, 0xaf, 0x7e, 0x17, 0xe0, 0xed, 0xaf, 0xb5,
	0xdd, 0x70, 0xf9, 0x5b, 0x9f, 0x9f, 0x78, 0x31, 0x6f, 0x7d, 0xc8, 0xd0, 0x4c, 0x90, 0x6f, 0x53,
	0x4e, 0xe3, 0x3c, 0x2a, 0x67, 0xf5, 0xf8, 0x5d, 0xb0, 0xf1, 0x01, 0x0c, 0x72, 0xc9, 0xe1, 0x11,
	0x4d, 0xb1, 0x20, 0x4a, 0xa6, 0x51, 0x1e, 0x97, 0xf3, 0xed, 0xb5, 0xe1, 0x6a, 0xf1, 0x33, 0xeb,
	0x95, 0x60, 0x5d, 0x47, 0x44, 0x4d, 0xe4, 0xbe, 0x53, 0xb5, 0xcd, 0xc1, 0x03, 0x9a, 0x52, 0x33,
	0x30, 0x31, 0x03, 0x2b, 0x77, 0x28, 0x7d, 0x65, 0xfb, 0x5e, 0x0d, 0x51, 0x93, 0xd8, 0x9d, 0x9b,
	0x87, 0xf7, 0xf4, 0x1f, 0x00, 0x00, 0xff, 0xff, 0x73, 0x07, 0x5a, 0x12, 0xb5, 0x02, 0x00, 0x00,
}
