// *****************************************************************************
// types.xs                                                           XL project
// *****************************************************************************
//
// File description:
//
//     Interface for the TYPES modules in XL
//
//
//
//
//
//
//
//
// *****************************************************************************
// This software is licensed under the GNU General Public License v3
// (C) 2019, Christophe de Dinechin <christophe@dinechin.org>
// *****************************************************************************
// This file is part of XL
//
// XL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// XL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with XL, in a file named COPYING.
// If not, see <https://www.gnu.org/licenses/>.
// *****************************************************************************

module TYPE                     // Representation of types

module COMBINATIONS             // Combinations oftypes
module LIFETIME                 // Lifetime of entities

module NUMBER                   // Common aspects of numbers
module ENUMERATED               // Enumerated values

module BOOLEAN                  // Boolean type (true / false)
module INTEGER                  // Signed and unsigned integer types
module REAL                     // Real numbers (floating-point representation)
module DECIMAL                  // Decimal floating-point numbers
module CHARACTER                // Representation of characters
module TEXT                     // Text (sequence of characters)

module CONVERSIONS              // Standard conversions

module RANGE                    // Range of elements (and range arithmetic)
module ITERATOR                 // Iterator

module UNSAFE                   // Unsafe data types
