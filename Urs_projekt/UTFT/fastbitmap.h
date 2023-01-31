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


void fastbitmap_1bit(uint16_t length, const uint16_t address, uint16_t fgcolor, uint16_t bg_color  ) asm ("fastbitmap_1bit");

void fastbitmap_16bit(uint16_t length, const uint16_t address ) asm ("fastbitmap_16bit");
void fastbitmap_pb565( const uint16_t address) asm("fastbitmap_pb565");