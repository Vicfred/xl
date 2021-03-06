// ****************************************************************************
//  xl.semantics.modules.xs         (C) 1992-2004 Christophe de Dinechin (ddd) 
//                                                                 XL2 project 
// ****************************************************************************
// 
//   File Description:
// 
//     Implementation of modules and import statement
// 
// 
// 
// 
// 
// 
// 
// 
// ****************************************************************************
// This document is released under the GNU General Public License.
// See http://www.gnu.org/copyleft/gpl.html for details
// ****************************************************************************
// * File       : $RCSFile$
// * Revision   : $Revision$
// * Date       : $Date$
// ****************************************************************************

import SYM = XL.SYMBOLS

module XL.SEMANTICS.MODULES with

    modules_path : string of text
    procedure AddPath (path : text)
    procedure ReplacePath (oldPath : text; newPath : text)
    function AddBuiltins(Input : PT.tree) return PT.tree
    function InXlBuiltinsModule() return boolean 
    function GetSymbols (value : PT.tree) return SYM.symbol_table
