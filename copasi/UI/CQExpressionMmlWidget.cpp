// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/Attic/CQExpressionMmlWidget.cpp,v $
//   $Revision: 1.1 $
//   $Name:  $
//   $Author: pwilly $
//   $Date: 2008/04/18 08:32:58 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

/****************************************************************************
 ** Form implementation generated from reading ui file 'CQExpressionMmlWidget.ui'
 **
 ** Created: Thu Apr 17 14:37:52 2008
 **      by: The User Interface Compiler ($Id: CQExpressionMmlWidget.cpp,v 1.1 2008/04/18 08:32:58 pwilly Exp $)
 **
 ** WARNING! All changes made in this file will be lost!
 ****************************************************************************/

#include "CQExpressionMmlWidget.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "CQExpressionWidget.h"
#include "CQMmlScrollView.h"
#include "mml/qtmmlwidget.h"
#include "utilities/CCopasiException.h"
#include "CQExpressionMmlWidget.ui.h"
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
    0xa2, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0x9d, 0x93, 0x3b, 0x4b, 0x2b,
    0x41, 0x18, 0x86, 0x9f, 0x59, 0x76, 0x73, 0x8f, 0xd7, 0x28, 0x18, 0x05,
    0x51, 0x04, 0xe1, 0x80, 0x85, 0x1a, 0x2c, 0x16, 0x0b, 0x7f, 0x80, 0x17,
    0x30, 0x9d, 0x20, 0x08, 0x01, 0xb5, 0xb0, 0xb0, 0xf1, 0x57, 0x08, 0x36,
    0xda, 0x9c, 0x9c, 0x26, 0xa4, 0xb4, 0xb3, 0xb6, 0x10, 0x9b, 0xa9, 0x5c,
    0x41, 0x44, 0x10, 0x21, 0x95, 0x57, 0x10, 0xa3, 0x32, 0x12, 0x93, 0x78,
    0x59, 0x8b, 0x4d, 0x96, 0x4d, 0x34, 0xca, 0xf1, 0xab, 0x66, 0xbe, 0x99,
    0xe7, 0xfd, 0xde, 0x99, 0xf9, 0x46, 0x48, 0x29, 0xf9, 0x2e, 0xcc, 0x8c,
    0xcf, 0xf6, 0xce, 0xe5, 0x42, 0x59, 0x78, 0xe7, 0xfa, 0x4f, 0xe0, 0xf2,
    0xfc, 0x08, 0x00, 0xf7, 0xa5, 0x6a, 0xfe, 0xd0, 0xf6, 0x0a, 0x89, 0xaf,
    0x1c, 0x98, 0x19, 0x9f, 0xbd, 0xb5, 0x3a, 0xcc, 0x40, 0x54, 0xc3, 0xd0,
    0x20, 0x7b, 0x06, 0x47, 0x79, 0x50, 0x65, 0x78, 0xb5, 0x41, 0x17, 0x90,
    0xdb, 0xb3, 0x90, 0x0b, 0x65, 0xd1, 0xd0, 0x41, 0x8b, 0xa1, 0xd1, 0xea,
    0x83, 0xb1, 0x4e, 0xb8, 0x29, 0xc0, 0x76, 0xce, 0x81, 0xfd, 0x75, 0xc4,
    0x27, 0x81, 0x6a, 0xf5, 0x7c, 0x19, 0xce, 0x0b, 0xf0, 0x70, 0x09, 0x39,
    0x05, 0x83, 0xed, 0xd0, 0x11, 0x82, 0x80, 0x0e, 0xc5, 0x57, 0x20, 0x39,
    0x8a, 0x99, 0xb1, 0xec, 0x86, 0x0e, 0x2e, 0x9f, 0xe1, 0xb6, 0x04, 0x85,
    0x17, 0x67, 0x6c, 0x76, 0x43, 0x57, 0x18, 0x82, 0x06, 0x3c, 0xbf, 0x38,
    0x7b, 0x76, 0xbf, 0xbc, 0x44, 0xcb, 0xa2, 0x2b, 0x3c, 0xc2, 0xfd, 0x1b,
    0x5c, 0x3c, 0xc1, 0x5d, 0x11, 0xfa, 0x5a, 0xa0, 0xb7, 0x09, 0x7a, 0x22,
    0x10, 0xf5, 0x83, 0x2a, 0x81, 0x2a, 0xbc, 0x83, 0x65, 0x35, 0x7e, 0x85,
    0xb6, 0x00, 0xbc, 0x01, 0xcd, 0x41, 0x88, 0x87, 0xa1, 0x33, 0x04, 0xb1,
    0x10, 0xc4, 0x23, 0x70, 0xf5, 0x04, 0xb1, 0x90, 0x06, 0x80, 0x56, 0x0f,
    0xca, 0xcd, 0x21, 0x91, 0x9c, 0xfd, 0x87, 0xdf, 0x80, 0x9e, 0x28, 0x74,
    0x47, 0xa0, 0x3d, 0x08, 0x61, 0xc3, 0x59, 0xcf, 0x17, 0x9d, 0x7b, 0x18,
    0x9f, 0x48, 0x23, 0x37, 0x87, 0x6a, 0x5f, 0xe1, 0x38, 0x63, 0x56, 0x9a,
    0xe6, 0x2f, 0xa9, 0xde, 0x8a, 0xe0, 0x03, 0x3c, 0xbe, 0x3b, 0x50, 0xab,
    0x0e, 0x7f, 0x02, 0xb5, 0x05, 0x75, 0x2f, 0x3c, 0x99, 0x9c, 0x26, 0xde,
    0x3f, 0xc3, 0xd4, 0x69, 0x0a, 0x91, 0x70, 0xf2, 0xf6, 0xc1, 0x62, 0x0d,
    0x20, 0x12, 0x69, 0xd7, 0xa9, 0x2b, 0x50, 0x85, 0x63, 0x31, 0xc1, 0xf5,
    0x69, 0x0a, 0xa5, 0x60, 0x2e, 0xb0, 0xc4, 0xca, 0xba, 0x14, 0x22, 0x91,
    0xae, 0x6d, 0xe5, 0x0a, 0xe8, 0x3a, 0xf0, 0xc2, 0x77, 0x57, 0x3b, 0x28,
    0x05, 0x4a, 0x35, 0x06, 0xea, 0x43, 0x03, 0x3e, 0xc1, 0x1b, 0x59, 0x58,
    0x59, 0x97, 0xdf, 0x82, 0xae, 0xc0, 0xfe, 0x09, 0xbf, 0x86, 0xa1, 0xf2,
    0x99, 0xb6, 0xd6, 0x4c, 0xf7, 0x9c, 0xff, 0x03, 0x03, 0x7c, 0x00, 0x79,
    0x53, 0xa2, 0x24, 0xa1, 0xd8, 0xe8, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x49,
    0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
  };

static const unsigned char image2_data[] =
  {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xe0, 0x91, 0x06, 0x10, 0x00, 0x00, 0x00,
    0xcd, 0x49, 0x44, 0x41, 0x54, 0x18, 0x95, 0x6d, 0xce, 0xb1, 0x4b, 0x02,
    0x51, 0x00, 0xc7, 0xf1, 0xef, 0x7b, 0x5e, 0x22, 0x04, 0x77, 0xa7, 0x91,
    0xb7, 0x07, 0xa1, 0x83, 0x90, 0x10, 0x0d, 0xde, 0x28, 0x04, 0xe2, 0x76,
    0xdb, 0x99, 0xff, 0x8b, 0x7f, 0xc8, 0x4d, 0xe1, 0x0d, 0x81, 0x53, 0x5b,
    0x86, 0x8b, 0x43, 0xbc, 0x68, 0x69, 0x72, 0xb0, 0xa9, 0xd9, 0x3b, 0x87,
    0x0e, 0x8e, 0x3a, 0xb0, 0x7a, 0x0d, 0x91, 0xd0, 0xd3, 0xdf, 0xfc, 0xe1,
    0xf7, 0xfb, 0x09, 0xa5, 0x14, 0x66, 0x7c, 0xdf, 0xd7, 0x0e, 0x30, 0x2a,
    0x97, 0xe9, 0xcc, 0xe7, 0xc2, 0xda, 0x07, 0x5e, 0x1e, 0x5b, 0xbc, 0x65,
    0x9f, 0x5c, 0xf6, 0x96, 0x44, 0xdd, 0xae, 0x96, 0xfb, 0xc0, 0x2a, 0xd9,
    0xf0, 0xf0, 0x94, 0x33, 0x9b, 0x36, 0x09, 0x8b, 0x02, 0x69, 0x82, 0xf7,
    0x8f, 0x6f, 0xa2, 0x38, 0xe5, 0xa2, 0x7d, 0x88, 0xeb, 0xfc, 0x0e, 0x49,
    0x13, 0x8c, 0x27, 0x6b, 0xc2, 0xa0, 0x86, 0x57, 0x3f, 0xa0, 0xd1, 0x59,
    0xa0, 0x94, 0x12, 0x02, 0xd8, 0x4e, 0x44, 0x71, 0x4a, 0x18, 0xd4, 0x38,
    0x3d, 0xa9, 0x6c, 0x01, 0x80, 0xb8, 0x39, 0xb3, 0x75, 0x75, 0xe0, 0x72,
    0xff, 0x5a, 0x10, 0xf4, 0xab, 0xff, 0x1a, 0xfe, 0xae, 0x48, 0xf2, 0x2f,
    0x92, 0xdb, 0x8c, 0xa1, 0x28, 0xe1, 0x1d, 0x59, 0x3b, 0x00, 0x40, 0x00,
    0xfa, 0xfa, 0xdc, 0xe6, 0x58, 0x0b, 0xae, 0x9e, 0x33, 0xee, 0x0c, 0x00,
    0xf0, 0x03, 0x3a, 0x05, 0x58, 0x9b, 0xd4, 0xab, 0x74, 0xd1, 0x00, 0x00,
    0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
  };

/*
 *  Constructs a CQExpressionMmlWidget as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
CQExpressionMmlWidget::CQExpressionMmlWidget(QWidget* parent, const char* name, WFlags fl)
    : QWidget(parent, name, fl)
{
  QImage img;
  img.loadFromData(image0_data, sizeof(image0_data), "PNG");
  image0 = img;
  img.loadFromData(image1_data, sizeof(image1_data), "PNG");
  image1 = img;
  img.loadFromData(image2_data, sizeof(image2_data), "PNG");
  image2 = img;
  if (!name)
    setName("CQExpressionMmlWidget");
  CQExpressionMmlWidgetLayout = new QGridLayout(this, 1, 1, 0, 6, "CQExpressionMmlWidgetLayout");

  mpWidgetStackExpressionMml = new QWidgetStack(this, "mpWidgetStackExpressionMml");
  mpWidgetStackExpressionMml->setMinimumSize(QSize(0, 0));

  mpExpressionPage = new QWidget(mpWidgetStackExpressionMml, "mpExpressionPage");
  mpExpressionPageLayout = new QGridLayout(mpExpressionPage, 1, 1, 0, 6, "mpExpressionPageLayout");

  mpExpressionWidget = new CQExpressionWidget(mpExpressionPage, "mpExpressionWidget");

  mpExpressionPageLayout->addWidget(mpExpressionWidget, 0, 0);

  layout4 = new QVBoxLayout(0, 0, 6, "layout4");

  mpBtnExpressionObject = new QToolButton(mpExpressionPage, "mpBtnExpressionObject");
  mpBtnExpressionObject->setMaximumSize(QSize(20, 20));
  mpBtnExpressionObject->setIconSet(QIconSet(image0));
  mpBtnExpressionObject->setProperty("IconSet", QIconSet(image0));
  layout4->addWidget(mpBtnExpressionObject);

  mpBtnViewExpression = new QToolButton(mpExpressionPage, "mpBtnViewExpression");
  mpBtnViewExpression->setEnabled(FALSE);
  mpBtnViewExpression->setMaximumSize(QSize(20, 20));
  mpBtnViewExpression->setIconSet(QIconSet(image1));
  layout4->addWidget(mpBtnViewExpression);
  mpSpacerExpressionObject = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
  layout4->addItem(mpSpacerExpressionObject);

  mpExpressionPageLayout->addLayout(layout4, 0, 1);
  mpWidgetStackExpressionMml->addWidget(mpExpressionPage, 0);

  mpMmlPage = new QWidget(mpWidgetStackExpressionMml, "mpMmlPage");
  mpMmlPageLayout = new QGridLayout(mpMmlPage, 1, 1, 0, 6, "mpMmlPageLayout");

  mpVBLayoutMml = new QVBoxLayout(0, 0, 6, "mpVBLayoutMml");

  mpBtnEditExpression = new QToolButton(mpMmlPage, "mpBtnEditExpression");
  mpBtnEditExpression->setMaximumSize(QSize(20, 20));
  mpBtnEditExpression->setIconSet(QIconSet(image2));
  mpVBLayoutMml->addWidget(mpBtnEditExpression);
  mpSpacerMmlObject = new QSpacerItem(20, 30, QSizePolicy::Minimum, QSizePolicy::Expanding);
  mpVBLayoutMml->addItem(mpSpacerMmlObject);

  mpMmlPageLayout->addLayout(mpVBLayoutMml, 0, 1);

  mpMmlScrollView = new CQMmlScrollView(mpMmlPage, "mpMmlScrollView");

  mpMmlPageLayout->addWidget(mpMmlScrollView, 0, 0);
  mpWidgetStackExpressionMml->addWidget(mpMmlPage, 1);

  CQExpressionMmlWidgetLayout->addWidget(mpWidgetStackExpressionMml, 0, 0);
  languageChange();
  resize(QSize(290, 125).expandedTo(minimumSizeHint()));
  clearWState(WState_Polished);

  // signals and slots connections
  connect(mpBtnEditExpression, SIGNAL(clicked()), this, SLOT(slotGoExpressionWidget()));
  connect(mpBtnExpressionObject, SIGNAL(clicked()), mpExpressionWidget, SLOT(slotSelectObject()));
  connect(mpBtnViewExpression, SIGNAL(clicked()), this, SLOT(updateWidget()));
  connect(mpExpressionWidget, SIGNAL(valid(bool)), mpBtnViewExpression, SLOT(setEnabled(bool)));
}

/*
 *  Destroys the object and frees any allocated resources
 */
CQExpressionMmlWidget::~CQExpressionMmlWidget()
{
  // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CQExpressionMmlWidget::languageChange()
{
  setCaption(tr("Expression-Mml Widget"));
  mpBtnExpressionObject->setText(QString::null);
  QToolTip::add(mpBtnExpressionObject, tr("select object"));
  mpBtnViewExpression->setText(QString::null);
  QToolTip::add(mpBtnViewExpression, tr("view expression"));
  mpBtnEditExpression->setText(QString::null);
  QToolTip::add(mpBtnEditExpression, tr("edit expression"));
}
