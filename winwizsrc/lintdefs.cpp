//  new rules for PC-Lint 9.00f
//lint -elib(14) 
//lint -elib(19) 
//lint -elib(40) 
//lint -elib(49) 
//lint -elib(64) 
//lint -elib(85)
//lint -elib(114) 
//lint -elib(129)
//lint -elib(1087)
//lint -esym(1065, _iob)

//lint -esym(715, hdcUnused)  // (line 572) not referenced

//lint -e1066  Symbol ... declared as "C" conflicts with line ...
//lint -e1708  typedef not declared as "C" conflicts with line ...
//lint -e1709  typedef ... declared as "C" 'conflicts with line ...
//lint -e1784  Symbol ... previously declared as "C", compare with line ...

//lint -e10    Expecting '}'
//lint -e18    Symbol ... conflicts with line ...
//lint -e31    Redefinition of symbol ...
//lint -e37    Value of enumerator ... inconsistent (conflicts with line ...)

//lint -esym(1714, CVListView::recalc_dx, CVListView::hide_horiz_scrollbar, CVListView::HitTest)
//lint -esym(1714, CVListView::GetItemState, CVListView::insert_column_header, CVListView::delete_column)
//lint -esym(1714, CTerminal::resize_terminal_pixels, CTerminal::get_terminal_dimens)
//lint -esym(1714, CTerminal::copy_list_to_clipboard, CTerminal::put)
//lint -esym(756, winproc_table_t)

//lint -e788   enum constant not used within defaulted switch

//lint -e1704  Constructor has private access specification
//lint -e1719  assignment operator for class has non-reference parameter
//lint -e1720  assignment operator for class has non-const parameter
//lint -e1722  assignment operator for class does not return a reference to class

//lint -e835   A zero has been given as left argument to operator '|'
//lint -e585   The sequence (???) is not a valid Trigraph sequence
//lint -e843   Variable could be declared as const
//lint -e1786  Implicit conversion to Boolean (assignment) (int to bool)
//lint -e1778  Assignment of string literal to variable is not const safe
//lint -e438   Last value assigned to variable not used
//lint -e844   Pointer variable could be declared as pointing to const
//lint -e838   Previously assigned value to variable has not been used
//lint -e840   Use of nul character in a string literal
//lint -e845   The right argument to operator '|' is certain to be 0

//lint -e730   Boolean argument to function that expects boolean argument (duh)
//lint -e786   String concatenation within initializer
//lint -e525   Negative indentation from line ...
//lint -e539   Did not expect positive indentation from line ...
//lint -e725   Expected positive indentation from line 1470
//lint -e716   while(1) ...
//lint -e717   do ... while(0);
//lint -e731   Boolean argument to equal/not equal
//lint -e801   Use of goto is deprecated
//lint -e818   Pointer parameter could be declared as pointing to const
//lint -e830   Location cited in prior message
//lint -e831   Reference cited in prior message
//lint -e834   Operator '-' followed by operator '-' is confusing.  Use parentheses.
//lint -e1776  Converting a string literal to char * is not const safe (arg. no. 2)
//lint -e1773  Attempt to cast away const (or volatile)
//lint -e1762  Member function could be made const

//lint -e768   global struct member not referenced
//lint -e534   Ignoring return value of function 
//lint -e755   global macro not referenced

//  compiler knows how to handle these, but maybe check later
//lint -e713   Loss of precision (assignment) (unsigned int to int)
//lint -e732   Loss of sign (initialization) (long to unsigned int)

