/*
  Copyright 2013(c) Ron Bessems All right reserved
  Latest version can be found at http://gizmogarage.net/fast-avr-utft/
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA 
 */


#ifndef FASTLINE_H_
#define FASTLINE_H_


 
 void fastlineANN(uint16_t color, uint16_t xa, uint16_t xb, uint16_t ya, uint16_t yb, uint16_t dx, uint16_t dy) asm ("fastlineANN");
 void fastlineANP(uint16_t color, uint16_t xa, uint16_t xb, uint16_t ya, uint16_t yb, uint16_t dx, uint16_t dy) asm ("fastlineANP");
 void fastlineAPN(uint16_t color, uint16_t xa, uint16_t xb, uint16_t ya, uint16_t yb, uint16_t dx, uint16_t dy) asm ("fastlineAPN");
 void fastlineAPP(uint16_t color, uint16_t xa, uint16_t xb, uint16_t ya, uint16_t yb, uint16_t dx, uint16_t dy) asm ("fastlineAPP");
 void fastlineBNN(uint16_t color, uint16_t xa, uint16_t xb, uint16_t ya, uint16_t yb, uint16_t dx, uint16_t dy) asm ("fastlineBNN");
 void fastlineBNP(uint16_t color, uint16_t xa, uint16_t xb, uint16_t ya, uint16_t yb, uint16_t dx, uint16_t dy) asm ("fastlineBNP");
 void fastlineBPN(uint16_t color, uint16_t xa, uint16_t xb, uint16_t ya, uint16_t yb, uint16_t dx, uint16_t dy) asm ("fastlineBPN");
 void fastlineBPP(uint16_t color, uint16_t xa, uint16_t xb, uint16_t ya, uint16_t yb, uint16_t dx, uint16_t dy) asm ("fastlineBPP");



#endif /* FASTLINE_H_ */