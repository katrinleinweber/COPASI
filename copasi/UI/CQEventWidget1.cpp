// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/CQEventWidget1.cpp,v $
//   $Revision: 1.6 $
//   $Name:  $
//   $Author: pwilly $
//   $Date: 2008/06/25 07:34:33 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

/****************************************************************************
 ** Form implementation generated from reading ui file 'CQEventWidget1.ui'
 **
 ** Created: Tue Jun 24 08:52:39 2008
 **      by: The User Interface Compiler ($Id: CQEventWidget1.cpp,v 1.6 2008/06/25 07:34:33 pwilly Exp $)
 **
 ** WARNING! All changes made in this file will be lost!
 ****************************************************************************/

#include "CQEventWidget1.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qlistbox.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "MyLineEdit.h"
#include "CQExpressionMmlWidget.h"
#include "CQExpressionWidget.h"
#include "CQEventWidget1.ui.h"
static const unsigned char image0_data[] =
  {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x02,
    0x51, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0x8d, 0x93, 0xdb, 0x53, 0x52,
    0x51, 0x14, 0x87, 0x7f, 0x1b, 0x37, 0x17, 0xe3, 0x2e, 0x8e, 0x98, 0x97,
    0x22, 0x01, 0x51, 0xb0, 0xcb, 0x28, 0x8c, 0x3a, 0xa6, 0x65, 0x4d, 0x33,
    0xdd, 0xa6, 0x9e, 0x7d, 0xe9, 0xef, 0xeb, 0xc5, 0xe9, 0xb1, 0x97, 0xa6,
    0x51, 0x31, 0x72, 0x72, 0x86, 0xd1, 0x20, 0x06, 0x27, 0x27, 0xec, 0x88,
    0xe0, 0x81, 0x40, 0xe0, 0x40, 0x9c, 0xa3, 0x70, 0x56, 0x0f, 0x8d, 0x34,
    0x27, 0xa3, 0xda, 0x8f, 0x7b, 0xad, 0xef, 0x9b, 0xbd, 0xd6, 0xda, 0x8b,
    0xc5, 0x62, 0x31, 0xfc, 0xcf, 0x29, 0x88, 0x97, 0x49, 0x91, 0x55, 0xd8,
    0x6c, 0x7a, 0xd8, 0x9d, 0x59, 0x76, 0x7e, 0xcf, 0xff, 0x06, 0x7d, 0xd9,
    0x1f, 0x20, 0xe1, 0xa0, 0x81, 0x7c, 0xae, 0x89, 0x62, 0x51, 0x00, 0xd3,
    0x31, 0x78, 0xbd, 0x66, 0x3c, 0x7e, 0xf6, 0x2b, 0xa7, 0xab, 0x60, 0xfd,
    0xad, 0x99, 0xd2, 0xa9, 0x1c, 0x0c, 0x06, 0x8e, 0xc9, 0x29, 0x07, 0x96,
    0xee, 0x98, 0x30, 0x7c, 0xf5, 0x98, 0x01, 0x8a, 0x26, 0xef, 0x8f, 0x82,
    0xd5, 0x97, 0x7a, 0x3a, 0x3a, 0xaa, 0xc2, 0xeb, 0xb3, 0xe0, 0xc9, 0x73,
    0x99, 0x01, 0x95, 0xae, 0xaf, 0xd4, 0x08, 0x92, 0xbb, 0x0e, 0xfa, 0xf0,
    0xfe, 0x04, 0x92, 0xd4, 0xc4, 0x74, 0xd8, 0x89, 0xc5, 0x65, 0x89, 0x75,
    0x03, 0x2f, 0x08, 0x76, 0xe3, 0x26, 0xda, 0x8c, 0x0a, 0x30, 0x1a, 0x2d,
    0x58, 0x5c, 0xee, 0xc7, 0x74, 0xb8, 0xf2, 0x4f, 0x58, 0x23, 0xd8, 0x8a,
    0x89, 0xb0, 0x5a, 0x55, 0xcc, 0x2f, 0x38, 0xe0, 0x0f, 0x5c, 0x84, 0xbd,
    0x8d, 0x0c, 0x29, 0xcd, 0x1a, 0x94, 0x96, 0x04, 0xa6, 0xd7, 0xa3, 0xe0,
    0x9a, 0x65, 0x1d, 0xc1, 0xab, 0xd5, 0x0a, 0xa9, 0x44, 0x88, 0xcc, 0x8e,
    0xc2, 0x1f, 0xa8, 0x69, 0xe0, 0xa1, 0xbd, 0x75, 0x2a, 0xa5, 0x13, 0x48,
    0x89, 0x39, 0x54, 0x65, 0x05, 0x27, 0x44, 0x60, 0x1c, 0x18, 0xf6, 0x44,
    0x69, 0xe2, 0xe1, 0x0a, 0x78, 0xee, 0x90, 0xd3, 0xe1, 0xd7, 0x1c, 0x66,
    0xc2, 0x7e, 0x4c, 0x86, 0xea, 0x1d, 0x78, 0xbc, 0x16, 0x27, 0x31, 0x16,
    0xc5, 0x5e, 0x3a, 0x8b, 0xfc, 0x99, 0x05, 0xad, 0xc1, 0x29, 0xb8, 0x6f,
    0x78, 0xe0, 0xeb, 0xb7, 0xe2, 0xb4, 0x94, 0xc1, 0x51, 0x72, 0x1b, 0x7d,
    0x87, 0x19, 0xf0, 0x5a, 0x59, 0xc4, 0x40, 0xdf, 0x19, 0x16, 0x96, 0xf4,
    0x1d, 0x78, 0xac, 0xbc, 0x45, 0xc2, 0xc6, 0x6b, 0x64, 0x04, 0xa0, 0x7d,
    0x65, 0x1e, 0x9e, 0xd0, 0x2d, 0x18, 0x02, 0x63, 0x9d, 0x78, 0x9b, 0x19,
    0xe9, 0x58, 0x4e, 0xc0, 0x5e, 0xd6, 0x81, 0xab, 0x6d, 0x05, 0x0e, 0xab,
    0xaa, 0xa9, 0xb7, 0x94, 0x58, 0xc7, 0x27, 0xc1, 0x08, 0x43, 0xe8, 0x01,
    0x46, 0xef, 0x47, 0x2e, 0xf4, 0x23, 0xba, 0x16, 0x47, 0x51, 0x54, 0xe0,
    0x33, 0xb9, 0xc0, 0x0d, 0x66, 0x2b, 0xc4, 0x92, 0xd0, 0x09, 0x06, 0x75,
    0x7b, 0x94, 0x3a, 0x29, 0x20, 0xab, 0x8b, 0x20, 0x12, 0xf0, 0x6b, 0xc0,
    0x6a, 0x2a, 0x4e, 0x3b, 0xef, 0xb6, 0x90, 0xfd, 0x9c, 0x87, 0x77, 0x2a,
    0x88, 0xc1, 0x60, 0x80, 0x71, 0xa7, 0xc3, 0x0e, 0x55, 0xd5, 0x61, 0x73,
    0xad, 0x44, 0xb7, 0xef, 0xba, 0x58, 0x93, 0xf7, 0xc3, 0x64, 0x33, 0xa3,
    0xb7, 0x95, 0x06, 0x09, 0x26, 0xd8, 0x64, 0x23, 0x49, 0x55, 0x09, 0x89,
    0x74, 0x01, 0xfb, 0xe9, 0x03, 0xc8, 0x0a, 0xc7, 0xa8, 0xef, 0x26, 0xee,
    0xbd, 0x58, 0xf9, 0x39, 0x05, 0x97, 0x5b, 0x65, 0x81, 0xe0, 0x35, 0x4a,
    0x7e, 0x14, 0x20, 0x35, 0x38, 0xcd, 0x5c, 0x77, 0xc2, 0xee, 0x7e, 0x04,
    0xa7, 0x6d, 0x03, 0x3b, 0x6f, 0x36, 0xb1, 0xad, 0xb4, 0xd1, 0x62, 0x3d,
    0xa8, 0xcb, 0x3d, 0x20, 0xdb, 0x08, 0x7c, 0xb3, 0x73, 0x08, 0x3f, 0x9d,
    0xeb, 0x94, 0xc5, 0xce, 0xb7, 0x31, 0xbe, 0xfd, 0x9d, 0x12, 0xc9, 0x6f,
    0x28, 0x8a, 0x15, 0xb0, 0xb3, 0x32, 0x5a, 0x8d, 0x3a, 0xea, 0xd5, 0x53,
    0xf0, 0x1e, 0x60, 0x68, 0xc4, 0x82, 0xf1, 0x80, 0x1b, 0x13, 0xa1, 0x11,
    0xb8, 0xc7, 0x5d, 0x9a, 0x9e, 0xb0, 0xdf, 0xd7, 0x39, 0x9f, 0x33, 0x52,
    0xa3, 0xde, 0x00, 0xb5, 0x75, 0xe0, 0x9c, 0xa3, 0xf7, 0x92, 0x01, 0x83,
    0xc3, 0x72, 0xd7, 0x5f, 0xf9, 0x03, 0x51, 0x05, 0xf9, 0xb7, 0x17, 0xf5,
    0x66, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42,
    0x60, 0x82
  };

static const unsigned char image1_data[] =
  {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x01,
    0x62, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0x95, 0x93, 0x4f, 0x4a, 0x03,
    0x31, 0x18, 0xc5, 0x7f, 0xc9, 0xa4, 0x2d, 0xa5, 0x82, 0x68, 0x41, 0x41,
    0x10, 0xbb, 0x70, 0x27, 0x5e, 0x20, 0x22, 0x82, 0xe8, 0x4e, 0xef, 0x50,
    0x2f, 0xe1, 0xce, 0x85, 0x2b, 0xaf, 0xa0, 0x2b, 0x11, 0xa1, 0xa8, 0x0b,
    0x17, 0x9e, 0xa1, 0xce, 0xc2, 0xb5, 0x3d, 0x80, 0x16, 0xb5, 0x05, 0xc5,
    0x96, 0x8a, 0x58, 0x6d, 0xf3, 0xb9, 0x98, 0x99, 0xfe, 0x9b, 0xa9, 0xd4,
    0x07, 0x21, 0xf9, 0xc2, 0xcb, 0xcb, 0x9b, 0x37, 0x5f, 0x94, 0xef, 0xfb,
    0x8c, 0xc2, 0x9e, 0x5a, 0xe1, 0x1b, 0x70, 0xe1, 0x86, 0x06, 0xd2, 0xe0,
    0xef, 0xf9, 0x6a, 0x94, 0x6b, 0x62, 0xa7, 0x01, 0xda, 0x20, 0x67, 0xc3,
    0x5b, 0xaa, 0x98, 0xc8, 0x1c, 0x23, 0x20, 0xc1, 0x94, 0xbf, 0x5e, 0xe2,
    0xb3, 0xf9, 0xca, 0xe6, 0x72, 0x01, 0x54, 0xe5, 0x1f, 0x02, 0x21, 0x72,
    0xc6, 0xa1, 0x53, 0x42, 0xd6, 0x13, 0x88, 0x99, 0x0f, 0xa0, 0xff, 0x12,
    0x98, 0x04, 0x26, 0x16, 0x18, 0x40, 0x7b, 0x98, 0x24, 0x08, 0xfc, 0x80,
    0x3d, 0xb6, 0x12, 0x7d, 0x5e, 0x14, 0xac, 0x49, 0x0a, 0xac, 0x67, 0x4f,
    0x85, 0xbe, 0xbb, 0xc2, 0xee, 0xbe, 0xe5, 0x66, 0x75, 0xf8, 0x8f, 0xa9,
    0x22, 0x98, 0x48, 0x51, 0x9f, 0xe7, 0x99, 0x1a, 0x48, 0x44, 0x21, 0x64,
    0x4d, 0x8b, 0x54, 0x26, 0xc3, 0x6d, 0xad, 0x8e, 0x7b, 0xae, 0x31, 0x7d,
    0x3f, 0x8b, 0x84, 0x61, 0x78, 0x06, 0xf0, 0xde, 0xfa, 0x21, 0xaa, 0xf6,
    0x07, 0xba, 0x2b, 0xa1, 0x65, 0x70, 0xce, 0xf1, 0xa5, 0x35, 0x28, 0x85,
    0x48, 0x70, 0x2c, 0x1a, 0x81, 0xab, 0x60, 0x65, 0xf0, 0x82, 0x7a, 0x65,
    0x61, 0x8e, 0x9c, 0x71, 0x20, 0xd0, 0xe9, 0x38, 0x76, 0xd6, 0xd6, 0x29,
    0x95, 0xcb, 0xbc, 0x37, 0x1a, 0x6c, 0x14, 0x0a, 0x3c, 0xb6, 0x9a, 0x68,
    0xdd, 0x57, 0xd0, 0x9e, 0xc6, 0xd7, 0x55, 0x0c, 0x75, 0x50, 0xdb, 0x80,
    0xae, 0xd2, 0xbb, 0x7e, 0x1e, 0xb6, 0xac, 0xc2, 0x49, 0x50, 0x8a, 0x16,
    0xee, 0x2e, 0x9e, 0xe0, 0x65, 0xc0, 0x82, 0x02, 0x3c, 0x30, 0xfe, 0x41,
    0xbc, 0x3d, 0xed, 0x89, 0x95, 0xa3, 0x99, 0x4b, 0x4a, 0xb2, 0xd8, 0x67,
    0x1b, 0xf0, 0x0f, 0xe3, 0xdc, 0xe4, 0x3e, 0x90, 0xa1, 0x29, 0xa1, 0x98,
    0x50, 0x60, 0x12, 0x24, 0x0b, 0x84, 0xc1, 0x9a, 0xb4, 0x46, 0xa7, 0x14,
    0x5e, 0x5a, 0x8d, 0xed, 0xd9, 0xe4, 0xb7, 0xd0, 0x0b, 0xf6, 0x01, 0x14,
    0x5c, 0x49, 0x65, 0xec, 0xab, 0xf9, 0x05, 0x38, 0x3f, 0x6e, 0x47, 0xd8,
    0x50, 0x6b, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae,
    0x42, 0x60, 0x82
  };

static const unsigned char image2_data[] =
  {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x01,
    0x1c, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xa5, 0x92, 0xb1, 0x6a, 0xc3,
    0x30, 0x14, 0x45, 0x8f, 0x8c, 0xdb, 0x82, 0xe9, 0xda, 0x31, 0x05, 0x8f,
    0x9d, 0x8d, 0x28, 0xf8, 0x1b, 0xf2, 0x0b, 0x86, 0x40, 0x86, 0x2e, 0xdd,
    0xfa, 0x1d, 0xdd, 0x32, 0x76, 0x08, 0xf8, 0x0f, 0x42, 0xc1, 0x7f, 0xf0,
    0x20, 0x94, 0xcc, 0xde, 0xb3, 0x06, 0x3a, 0x15, 0x43, 0xda, 0x50, 0x75,
    0x50, 0xac, 0xca, 0x96, 0xd2, 0x52, 0xfa, 0x40, 0x3c, 0xa4, 0xfb, 0xce,
    0xc5, 0x3c, 0x5f, 0x25, 0x22, 0xfc, 0xa7, 0x12, 0xff, 0x92, 0xce, 0x4b,
    0x93, 0xce, 0x4b, 0x73, 0x6a, 0x38, 0xa6, 0x27, 0xbe, 0xa8, 0x6b, 0x83,
    0xae, 0x0d, 0x31, 0x93, 0x53, 0x7a, 0xe2, 0x8b, 0x34, 0x39, 0x34, 0x79,
    0x30, 0xf4, 0x93, 0x9e, 0x02, 0xac, 0x5b, 0xd0, 0x4d, 0x6e, 0xa7, 0x3f,
    0x0e, 0xb0, 0x9a, 0xa0, 0x6b, 0xc3, 0x4b, 0xa5, 0x0c, 0x80, 0xae, 0x0d,
    0xac, 0x26, 0x56, 0x3f, 0x4b, 0xa1, 0xc9, 0x59, 0xb7, 0x50, 0x00, 0xaa,
    0x5f, 0xe2, 0xa6, 0x2c, 0xcd, 0xfd, 0xc3, 0xb5, 0x1d, 0xda, 0x77, 0xb6,
    0xcf, 0x76, 0xb6, 0x2f, 0xaf, 0x6c, 0xbf, 0xc8, 0x00, 0x58, 0x3c, 0x6e,
    0x29, 0x44, 0x14, 0xbe, 0x81, 0x33, 0x99, 0x1d, 0x87, 0xdf, 0xbb, 0xe1,
    0x12, 0xce, 0x8f, 0xf0, 0x72, 0xe7, 0xe0, 0xc0, 0xc0, 0x99, 0x4c, 0x2f,
    0xc7, 0x3b, 0xb4, 0xf0, 0xf3, 0xdb, 0x00, 0x76, 0x3b, 0xf0, 0xeb, 0xf6,
    0x06, 0x78, 0xed, 0xc6, 0xcf, 0x4e, 0x3b, 0x8c, 0xde, 0x82, 0x1c, 0xe8,
    0xda, 0xc0, 0xfe, 0x33, 0x7a, 0x62, 0xbf, 0x38, 0xc8, 0x01, 0xd5, 0xe0,
    0x0b, 0x87, 0x55, 0xa9, 0x5f, 0x72, 0x30, 0x82, 0x17, 0x1b, 0x7b, 0x5c,
    0x65, 0xa1, 0x49, 0x02, 0x36, 0x07, 0x0e, 0xce, 0xbe, 0xe1, 0x42, 0x44,
    0x15, 0x22, 0x2a, 0x30, 0xb9, 0x53, 0x96, 0xe9, 0x0d, 0x0a, 0x11, 0xf5,
    0xd4, 0x86, 0x70, 0xcf, 0x8c, 0x4d, 0x7c, 0x3d, 0xc8, 0x41, 0x0f, 0xc4,
    0x56, 0x10, 0xd3, 0x83, 0x1c, 0xfc, 0xb5, 0xbe, 0x00, 0x40, 0xce, 0xa4,
    0xe3, 0x14, 0x6f, 0x5e, 0xa8, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e,
    0x44, 0xae, 0x42, 0x60, 0x82
  };

/*
 *  Constructs a CQEventWidget1 which is a child of 'parent', with the
 *  name 'name'.'
 */
CQEventWidget1::CQEventWidget1(QWidget* parent, const char* name)
    : CopasiWidget(parent, name)
{
  QImage img;
  img.loadFromData(image0_data, sizeof(image0_data), "PNG");
  image0 = img;
  img.loadFromData(image1_data, sizeof(image1_data), "PNG");
  image1 = img;
  img.loadFromData(image2_data, sizeof(image2_data), "PNG");
  image2 = img;
  if (!name)
    setName("CQEventWidget1");
  setSizePolicy(QSizePolicy((QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, sizePolicy().hasHeightForWidth()));
  CQEventWidget1Layout = new QVBoxLayout(this, 11, 6, "CQEventWidget1Layout");

  layout31 = new QGridLayout(0, 1, 1, 0, 6, "layout31");

  mpLabelDelay = new QLabel(this, "mpLabelDelay");
  mpLabelDelay->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, mpLabelDelay->sizePolicy().hasHeightForWidth()));
  mpLabelDelay->setAlignment(int(QLabel::AlignTop | QLabel::AlignRight));

  layout31->addWidget(mpLabelDelay, 3, 0);

  mpLineEditName = new MyLineEdit(this, "mpLineEditName");
  mpLineEditName->setMinimumSize(QSize(380, 20));
  mpLineEditName->setMaximumSize(QSize(32767, 32767));

  layout31->addWidget(mpLineEditName, 0, 1);

  mpExpressionDelay = new CQExpressionMmlWidget(this, "mpExpressionDelay");
  mpExpressionDelay->setMinimumSize(QSize(380, 100));

  layout31->addWidget(mpExpressionDelay, 3, 1);

  mpExpressionTrigger = new CQExpressionMmlWidget(this, "mpExpressionTrigger");
  mpExpressionTrigger->setMinimumSize(QSize(380, 120));
  mpExpressionTrigger->setMaximumSize(QSize(32767, 32767));

  layout31->addWidget(mpExpressionTrigger, 1, 1);

  mpLabelEventName = new QLabel(this, "mpLabelEventName");
  mpLabelEventName->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, mpLabelEventName->sizePolicy().hasHeightForWidth()));
  mpLabelEventName->setAlignment(int(QLabel::AlignVCenter | QLabel::AlignRight));

  layout31->addWidget(mpLabelEventName, 0, 0);

  mpLabelTrigger = new QLabel(this, "mpLabelTrigger");
  mpLabelTrigger->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, mpLabelTrigger->sizePolicy().hasHeightForWidth()));
  mpLabelTrigger->setAlignment(int(QLabel::AlignTop | QLabel::AlignRight));

  layout31->addWidget(mpLabelTrigger, 1, 0);

  mpCheckBoxDelay = new QCheckBox(this, "mpCheckBoxDelay");

  layout31->addMultiCellWidget(mpCheckBoxDelay, 2, 2, 0, 1);
  CQEventWidget1Layout->addLayout(layout31);

  mpLine1 = new QFrame(this, "mpLine1");
  mpLine1->setFrameShape(QFrame::HLine);
  mpLine1->setFrameShadow(QFrame::Sunken);
  mpLine1->setFrameShape(QFrame::HLine);
  CQEventWidget1Layout->addWidget(mpLine1);

  layout13 = new QGridLayout(0, 1, 1, 0, 6, "layout13");

  layout12 = new QHBoxLayout(0, 0, 6, "layout12");

  mpLBTarget = new QListBox(this, "mpLBTarget");
  layout12->addWidget(mpLBTarget);

  layout11 = new QVBoxLayout(0, 0, 6, "layout11");

  mpBtnSelectObject = new QToolButton(this, "mpBtnSelectObject");
  mpBtnSelectObject->setMaximumSize(QSize(20, 20));
  mpBtnSelectObject->setIconSet(QIconSet(image0));
  layout11->addWidget(mpBtnSelectObject);

  mpBtnAddTarget = new QToolButton(this, "mpBtnAddTarget");
  mpBtnAddTarget->setMaximumSize(QSize(20, 20));
  mpBtnAddTarget->setIconSet(QIconSet(image1));
  layout11->addWidget(mpBtnAddTarget);

  mpBtnDeleteTarget = new QToolButton(this, "mpBtnDeleteTarget");
  mpBtnDeleteTarget->setMaximumSize(QSize(20, 20));
  mpBtnDeleteTarget->setIconSet(QIconSet(image2));
  layout11->addWidget(mpBtnDeleteTarget);
  spacerTargetIcons = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  layout11->addItem(spacerTargetIcons);
  layout12->addLayout(layout11);

  layout13->addLayout(layout12, 1, 0);

  mpExpressionEA = new CQExpressionMmlWidget(this, "mpExpressionEA");
  mpExpressionEA->setMinimumSize(QSize(300, 60));

  layout13->addWidget(mpExpressionEA, 1, 1);

  layout13_2 = new QHBoxLayout(0, 0, 6, "layout13_2");

  mpLabelTarget = new QLabel(this, "mpLabelTarget");
  mpLabelTarget->setAlignment(int(QLabel::AlignVCenter | QLabel::AlignRight));
  layout13_2->addWidget(mpLabelTarget);
  spacerTarget = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  layout13_2->addItem(spacerTarget);

  layout13->addLayout(layout13_2, 0, 0);

  layout17 = new QHBoxLayout(0, 0, 6, "layout17");

  mpLabelEA = new QLabel(this, "mpLabelEA");
  mpLabelEA->setAlignment(int(QLabel::AlignVCenter | QLabel::AlignRight));
  layout17->addWidget(mpLabelEA);
  spacerExpression = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  layout17->addItem(spacerExpression);

  layout13->addLayout(layout17, 0, 1);
  CQEventWidget1Layout->addLayout(layout13);

  mpLine2 = new QFrame(this, "mpLine2");
  mpLine2->setFrameShape(QFrame::HLine);
  mpLine2->setFrameShadow(QFrame::Sunken);
  mpLine2->setFrameShape(QFrame::HLine);
  CQEventWidget1Layout->addWidget(mpLine2);

  mpBtnLayout = new QHBoxLayout(0, 0, 6, "mpBtnLayout");

  mpBtnCommit = new QPushButton(this, "mpBtnCommit");
  mpBtnLayout->addWidget(mpBtnCommit);

  mpBtnRevert = new QPushButton(this, "mpBtnRevert");
  mpBtnLayout->addWidget(mpBtnRevert);

  mpBtnNew = new QPushButton(this, "mpBtnNew");
  mpBtnLayout->addWidget(mpBtnNew);

  mpBtnDelete = new QPushButton(this, "mpBtnDelete");
  mpBtnLayout->addWidget(mpBtnDelete);
  CQEventWidget1Layout->addLayout(mpBtnLayout);
  languageChange();
  resize(QSize(513, 498).expandedTo(minimumSizeHint()));
  clearWState(WState_Polished);

  // signals and slots connections
  connect(mpBtnCommit, SIGNAL(clicked()), this, SLOT(slotBtnCommitClicked()));
  connect(mpBtnDelete, SIGNAL(clicked()), this, SLOT(slotBtnDeleteClicked()));
  connect(mpBtnNew, SIGNAL(clicked()), this, SLOT(slotBtnNewClicked()));
  connect(mpBtnRevert, SIGNAL(clicked()), this, SLOT(slotBtnRevertClicked()));
  connect(mpBtnAddTarget, SIGNAL(clicked()), this, SLOT(slotAddTarget()));
  connect(mpBtnDeleteTarget, SIGNAL(clicked()), this, SLOT(slotDeleteTarget()));
  connect(mpBtnSelectObject, SIGNAL(clicked()), this, SLOT(slotSelectObject()));
  connect(mpCheckBoxDelay, SIGNAL(toggled(bool)), this, SLOT(slotApplyDelay(bool)));

  // tab order
  setTabOrder(mpBtnCommit, mpBtnRevert);
  setTabOrder(mpBtnRevert, mpBtnNew);
  setTabOrder(mpBtnNew, mpBtnDelete);
  init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
CQEventWidget1::~CQEventWidget1()
{
  // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CQEventWidget1::languageChange()
{
  setCaption(tr("Event Widget"));
  mpLabelDelay->setText(tr("Delay Expression"));
  mpLabelEventName->setText(tr("Event Name"));
  mpLabelTrigger->setText(tr("Condition Trigger"));
  mpCheckBoxDelay->setText(tr("enabled delay"));
  mpBtnSelectObject->setText(QString::null);
  QToolTip::add(mpBtnSelectObject, tr("edit target"));
  mpBtnAddTarget->setText(QString::null);
  QToolTip::add(mpBtnAddTarget, tr("add target"));
  mpBtnDeleteTarget->setText(QString::null);
  QToolTip::add(mpBtnDeleteTarget, tr("delete target"));
  mpLabelTarget->setText(tr("Target"));
  mpLabelEA->setText(tr("Expression"));
  mpBtnCommit->setText(tr("Commit"));
  mpBtnRevert->setText(tr("Revert"));
  mpBtnNew->setText(tr("New"));
  mpBtnDelete->setText(tr("Delete"));
}
