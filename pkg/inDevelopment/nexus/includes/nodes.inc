use os;
use datafile;

include ":datafile:datafile";
include ":gumps:yesno";
include ":yggdrasil:boss";
include "include/arrays";

/*=============================================
=        Nodes functions code            =
=============================================*/


function LoadNexusNodeData()
    var df := DFOpenDataFile( ":nexus:node", DF_CREATE, DF_KEYTYPE_STRING);
    return df;
endfunction

