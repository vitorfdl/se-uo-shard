/** $Id: arrays.inc 1433 2006-05-18 21:48:04Z austin $
 *
 * Purpose
 * Provide various functions for array handling.
 *
 */
use basic;
use math;
use os;
use uo;

/*
 * GetArrayRange(the_array, start, stop)
 *
 * Purpose
 * Retrieves a range from the array.
 *
 * Parameters
 * the_array:	The array to extract data from.
 * start:	Start index to get data from.
 * stop:	End index to get data from.
 *		If stop is 0, will go until the end of the array is reached.
 *
 * Return value
 * Returns an array on success.
 * Returns an error on failure.
 *
 */
function GetArrayRange(byref the_array, start, stop:=0)
	if ( Lower(TypeOf(the_array)) != "array" )
		return error{"errortext":="Object passed was not an array."};
	endif

    start := CInt(start);
    stop  := CInt(stop);

	if ( start < 1 )
		start := 1;
	endif
	if ( !stop )
		stop := the_array.Size();
	elseif ( stop > the_array.Size() )
		stop := the_array.Size();
	endif

	var new_array := array{};
	for ( start; start<=stop; start:=start+1 )
		new_array.Append(the_array[start]);
		sleepms(2);
	endfor

	return new_array;
endfunction

/*
 * BreakArray(the_array, size)
 *
 * Purpose
 * Breaks an array up into smaller arrays.
 *
 * Parameters
 * the_array:	The array that will be broken up.
 * size:	Size of the new arrays.
 *
 * Return value
 * Returns an array of arrays
 *
 */
function BreakArray(byref the_array, size:=1)
	if ( Lower(TypeOf(the_array)) != "array" )
		return error{"errortext":="Object passed was not an array."};
	endif

	if ( the_array.Size() > size )
		var new_arrays := array();
		var arrays_needed := Ceil(CDbl(the_array.Size()) / CDbl(size));

		var i := 1;
		var start := i;
		var stop := size;
		for ( i; i<=arrays_needed; i:=i+1 )
			new_arrays.Append(GetArrayRange(the_array, start, stop));
			start := stop+1;
			stop := stop+size;
			sleepms(2);
		endfor

		return new_arrays;
	else
		return array{the_array};
	endif
endfunction

/*
 * FindInArray(byref the_array, find_what)
 *
 * Purpose
 * Finds the first instance of something in an array.
 *
 * Parameters
 * the_array:	Array to search.
 * find_what:	What to look for.
 *
 * Return value
 * Returns > 1 (the index) if it is found.
 * Returns 0 if 'find_what' was not found.
 *
 */
function FindInArray(byref the_array, find_what)
	if ( Lower(TypeOf(the_array)) != "array" )
		return error{"errortext":="Object passed was not an array."};
	endif

	foreach index in ( the_array )
        if ((TypeOf(find_what) == "Struct" || TypeOf(index) == "Struct") || (TypeOf(index) == "Dictionary" || TypeOf(find_what) == "Dictionary"))
            var can_return := array{};
            foreach key in (find_what.Keys())
                if (index.Exists(key) && find_what[key] == index[key])
                    can_return.append(1);
                else
                    can_return.append(2);
                endif
                sleepms(2);
            endforeach

            if (!(2 in can_return))
                return _index_iter;
            endif
		elseif ( index == find_what )
			return _index_iter;
		endif
		sleepms(2);
	endforeach

	return 0;
endfunction

/*
 * FindElemInArray(byref the_array, find_what)
 *
 * Purpose
 * Finds the first instance of something in an array.
 *
 * Parameters
 * the_array:   Array to search.
 * find_what:   What to look for.
 *
 * Return value
 * Returns the elem of what its searching
 * Returns 0 if 'find_what' was not found.
 *
 */
function FindElemInArray(byref the_array, find_what)
    if ( Lower(TypeOf(the_array)) != "array" )
        return error{"errortext":="Object passed was not an array."};
    endif

    foreach index in ( the_array )
        if ((TypeOf(find_what) == "Struct" || TypeOf(index) == "Struct") || (TypeOf(index) == "Dictionary" || TypeOf(find_what) == "Dictionary"))
            var can_return := array{};
            foreach key in (find_what.Keys())
                if (index.Exists(key) && find_what[key] == index[key])
                    can_return.append(1);
                else
                    can_return.append(2);
                endif
                sleepms(2);
            endforeach

            if (!(2 in can_return))
                return index;
            endif
        elseif ( index == find_what )
			return index;
		endif
        sleepms(2);
    endforeach

    return 0;
endfunction

/*
 * FindAllElemInArray(byref the_array, find_what)
 *
 * Purpose
 * Finds the all instances of something in an array.
 *
 * Parameters
 * the_array:   Array to search.
 * find_what:   What to look for.
 *
 * Return value
 * Returns all elem of what its searching
 * Returns 0 if 'find_what' was not found.
 *
 */
function FindAllElemInArray(byref the_array, find_what, return_index := 0)
    if ( Lower(TypeOf(the_array)) != "array" )
        return error{"errortext":="Object passed was not an array."};
    endif

    var to_return := array{};
    foreach index in ( the_array )
        if ((TypeOf(find_what) == "Struct" || TypeOf(index) == "Struct") || (TypeOf(index) == "Dictionary" || TypeOf(find_what) == "Dictionary"))
            foreach key in (find_what.Keys())
                if (index.Exists(key) && find_what[key] == index[key])
					if (return_index)
                   		to_return.append(_index_iter);
					else
                   		to_return.append(index);
					endif
                endif
                sleepms(2);
            endforeach
        elseif ( index == find_what )
			return to_return.append(index);
        endif
        sleepms(2);
    endforeach

	if (to_return.size() >= 1)
		return to_return;
	endif
	
    return 0;
endfunction


/*
 * RemoveFromArray(byref the_array, find_what)
 *
 * Purpose
 * Finds the first instance of something in an array.
 *
 * Parameters
 * the_array:	Array to search.
 * find_what:	What to look for.
 *
 * Return value
 * Returns > 1 (the index) if it is found.
 * Returns 0 if 'find_what' was not found.
 *
 */
function RemoveFromArray(the_array, delete_what)
	if ( Lower(TypeOf(delete_what)) != "array")
		delete_what := array{delete_what};
	endif
	
	var iter_to_remove := array{};
	foreach item in (the_array)
		if (item in delete_what)
			iter_to_remove += _item_iter;
		endif
		sleepms(5);
	endforeach

	iter_to_remove.Reverse();
	foreach iter in iter_to_remove
		the_array.Erase(iter);
	endforeach

	return the_array;
endfunction

/*
 * ReturnHighest(byref the_array)
 *
 * Purpose
 * Return Highest integer or real value from array
 *
 * Parameters
 * the_array:	Array to search.
 *
 * Return value
 * Returns the array without the Highest value
 *
 */
function ReturnHighest(the_array)
	if (the_array.Size() <= 1)
        return the_array;
	endif

	var i;
	var l_val := 1;
	for (i := 1; i < the_array.Size(); i+=1)
		if (the_array[l_val] > the_array[i])
			l_val := i;
		endif
	endfor

	return the_array[l_val];
endfunction

/*
 * ReturnLowest(byref the_array)
 *
 * Purpose
 * Return lowest integer or real value from array
 *
 * Parameters
 * the_array:	Array to search.
 *
 * Return value
 * Returns the array without the Lowest value
 *
 */
function ReturnLowest(the_array)
	if (the_array.Size() <= 1)
        return the_array;
	endif

	var i;
	var l_val := 1;
	for (i := 1; i < the_array.Size(); i+=1)
		if (the_array[l_val] < the_array[i])
			l_val := i;
		endif
	endfor

	return the_array[l_val];
endfunction

/*
 * ArraySum(byref the_array)
 *
 * Purpose
 * Sum all integer/real values inside the array.
 *
 * Parameters
 * the_array:	Array to sum.
 *
 * Return value
 * Returns the value of sum of the values of the array
 *
 */
function ArraySum(the_array)
	if (the_array.Size() <= 1)
        return the_array[1];
	endif

	var f_val := 0;
	foreach entry in (the_array)
		f_val += entry;
	endforeach

	return f_val;
endfunction

function SearchArrayOfModel(search_for, equal, to_search)
    var counter;
	foreach item in to_search
		var item_prop := item.get_member(search_for);
		if ( item_prop == equal)
			counter := 1;
		endif
      sleepms(2);
	endforeach

	if (!counter)
		return 0;
	endif

	return 1;
endfunction

function FindObjInModel(search_for, equal, to_search)
    var counter;
	foreach item in to_search
		var item_prop := item.get_member(search_for);
		if ( item_prop == equal)
			return item;
		endif
      sleepms(2);
	endforeach

	return 0;
endfunction

function removeIndexes(byref the_array, index_array)
   index_array.Reverse(); //cheat for pol
   foreach index in index_array
      the_array.erase(index);
      sleepms(2);
   endforeach
endfunction
