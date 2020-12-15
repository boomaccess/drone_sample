#ifndef _POSITION_H
#define _POSITION_H



int PositionNextPoint(POINTTYPE pos1, POINTTYPE pos2, POINTTYPE *pPos);
int PositionFootPerpendicular(POINTTYPE pos1, POINTTYPE pos2, double fDistance, POINTTYPE *pPos3, POINTTYPE *pPos4);


#endif