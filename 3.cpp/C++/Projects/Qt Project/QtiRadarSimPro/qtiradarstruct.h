#ifndef QTIRADARSTRUCT_H
#define QTIRADARSTRUCT_H
#include <QString>
#include <QStringList>
#include "PdwStruct/PdwStruct.hpp"

struct _QtPriPara
{
    _PriType PriType 	= PRI_FIX;
    size_t   dPri       = 10000;
    size_t   dRange     = 5000;
    size_t   nPriNum    = 10;
    size_t   nGroupNum 	= 1;
    size_t   PriArrLen 	= 0;
    size_t   dPriStep   = 1;
    size_t   PriArr[JAG_NUM] = {0};
};
struct _QtRfPara
{
    _RfType  RfType     = RF_FIX;
    size_t   dFre       = 256000;
    size_t   nFreNum	= 10;
    size_t   nGroupNum  = 1;
    size_t   RfArrLen   = 0;
    size_t   RfArr[AGI_NUM] = {0};
};
struct _QtPwPara
{
    _PwType  PwType     = PW_FIX;
    size_t   dPw        = 50;
    size_t   nPwNum     = 10;
    size_t   nGroupNum  = 1;
    size_t   PwArrLen   = 0;
    size_t   PwArr[PW_NUM] = {0};
};
struct _QtiRadarPara
{
    _QtPriPara  QtPriPara;
    _QtRfPara   QtRfPara;
    _QtPwPara   QtPwPara;
};
struct _QtPlatFormPara
{
    QString     TraceName;
    int         DoaInit;
    QStringList RadarNameList;
};
typedef _Pdw<UINT, UINT, UINT, UINT, UINT>  PdwType;

#endif // QTIRADARSTRUCT_H
