//$Id: FLSBooks.inc 386 2005-10-03 00:22:39Z muaddiblsd $

/*===============================================================
* Current Version
* FLSBOOKS.INC - v1.0
* Updated 14/12/2017 4:00PM
===============================================================*/

use os;
use cfgfile;

include ":gumps:gumps_ex";
/* SysBook_GetFLSBooksCfgFile()
 * 
 * Purpose
 * Loads fflsbooks.cfg
 *
 * Parameters
 *
 * Return value
 * Config file
 *
 */
function SysBook_GetFLSCfgFile()
	var cfg_file := ReadConfigFile(":sysbook:config/flsbooks");
	if ( cfg_file.errortext )
		SysLog("Error::SysBook_GetFLSBooksCfgFile() - Unable to open FLSBooks.cfg ->"+cfg_file.errortext);
	endif

	return cfg_file;
endfunction

/*
 * SysBook_GetFLSCfgElem(elem_name, cfg_file)
 *
 * Purpose
 * Retrieves a config elem for a book in FLSbooks.cfg
 *
 * Parameters
 * elem_name:		Name of the book elem to retrieve.
 * cfg_file:		Optional parameter - reference to a config already read in.
 *
 * Return value
 * Returns a config file elem.
 *
 */
function SysBook_GetFLSCfgElem(elem_name, byref cfg_file:=0)
	if ( !cfg_file )
		cfg_file := SysBook_GetFLSCfgFile();
	endif
	
	var cfg_elem := cfg_file[elem_name];
	if ( cfg_elem.errortext )
		SysLog("Error::SysBook_GetFLSCfgElem() - unable to find cfg elem ["+elem_name+"] ->"+cfg_elem.errortext);
	endif
	
	return cfg_elem;
endfunction

/*
 * SysBook_ParseFLSCfgElem(elem_name, byref cfg_file)
 *
 * Purpose
 * Parses the insanity that exists within FLSbooks.cfg and returns
 * a data structure that can be used with the method script of a book.
 *
 * Parameters
 * elem_name:		Name of the book elem to retrieve.
 * cfg_file:		Optional parameter - reference to a config already read in.
 *
 * Return value
 * Returns a config file elem.
 *
 */
function SysBook_ParseFLSCfgElem(elem_name, byref cfg_file := 0)
	if ( !cfg_file )
		cfg_file := SysBook_GetFLSCfgFile();
	endif
	
	var cfg_elem :=  SysBook_GetFLSCfgElem(elem_name, cfg_file);
	if ( !cfg_elem )
		return 0;
	endif
	
	var book_info := struct;
	book_info.+Author := cfg_elem.Author;
	book_info.+Title := cfg_elem.Title;
	book_info.+NumPages := CInt(cfg_elem.Pages);
	book_info.+BookType := cfg_elem.BookType;

	var raw_content := GetConfigStringArray(cfg_elem, "pl");
   var book_content := array;
   foreach paragraph in raw_content
      var lines := GFWordWrap(paragraph, 180);
      foreach line in lines
         book_content.append(line);
         sleepms(2);
      endforeach

      // if (_paragraph_iter != raw_content.size())
      //    book_content.append("");
      // endif
   endforeach
	
   book_info.+Contents := book_content;
	return book_info;
endfunction

function SysBook_CreateBook(bag, book_int)
   var book_info := SysBook_ParseFLSCfgElem(book_int);
   if (!book_info)
      return;
   endif

   var book := CreateItemInContainer(bag, "Book2", 1);
	if ( !book )
		Print("Unable to create book ->"+book.errortext);
	endif
	
	book.SetAuthor(book_info.Author);
	book.SetTitle(book_info.Title);

   var line := 1;
	foreach text_line in ( book_info.Contents )
		book.SetLine(_text_line_iter, text_line);
		sleepms(2);
	endforeach

   var page_num := Floor(book_info.Contents.size()/7);

   book.SetNumPages(page_num);
   book.ReadOnly(1);

   return book;
endfunction