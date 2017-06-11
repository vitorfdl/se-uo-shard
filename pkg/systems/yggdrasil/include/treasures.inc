use uo; 

function ModelTreasure( data := struct{} )
    if (!data.name)
        data.name := "";
    endif

    if (data.active != 0 && data.active != 1)
        data.active := 0;
    endif

    if (TypeOf(data.loots) != "Array")
        data.loots := array{};
    endif

    if (!data.level || data.level < 1 || data.level > 5)
        data.level := 1;
    endif

    if (data.protected != 1 && data.protected != 0)
        data.protected := 0;
    endif

    if (data.trapped != 1 && data.trapped != 0)
        data.trapped := 0;
    endif

    if (!data.serial)
        data.serial := 0x0;
    endif

    if (TypeOf(data.yggdrasil) != "Struct")
        data.yggdrasil := struct{};
    endif

    return data;
endfunction