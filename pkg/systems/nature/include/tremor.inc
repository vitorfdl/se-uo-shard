
use uo;
use util;
use vitals;

include ":charactercreation:resists";
include ":timedScripts:timedScripts";
include "include/damage";

function EfeitosTerremoto(who,danobase,danoale,dif)

         if (who.dead)
            return;
         endif
         
         if( who.cmdlevel > 0)
             if(!GetObjProperty (who, "lastpossessedserial"))
                 return;
             endif
         endif
         
         
         PlaySoundEffectPrivate( who, 0x222, who );
         SendSysMessageEx(who, "O solo nas proximidades treme violentamente.", SSM_INFO);
         
         if (CheckResist(who,  REFLEXOS, dif))//note: colocar uma variavel de chance de resistir ao terremoto
            PrintTextAbove( who, "*Atraves de movimentos acrobaticos habeis, resiste ao tremor*");
            PerformAction( who, 20);
         else

             TS_StartTimer(who, "paralysis", 2);
             PerformAction( who, 21);
             DamageFLS(who, danobase + (RandomInt(danoale)));
             var pedra;
             
             for (pedra := 1; pedra <=  (RandomInt( 5 ) + 1); pedra := pedra + 1)
             
                     var tempx :=  who.x + RandomIntMinMax( -4, 4);
                     var tempy :=  who.y + RandomIntMinMax( -4, 4);
                     
                     //SendSysMessageEx(who, "p2", SSM_INFO);
                     
                     if (CheckLosAt( who, tempx, tempy, who.z ) != 0 || !who.dead)
                        PlayMovingEffectXYZ( tempx, tempy,
                        who.z + 60, tempx, tempy, who.z, 0x1773, 0x9, 0xA1);
                        
                        CreateItemAtLocation( tempx, tempy, who.z, 0xBD12 );
                     endif
             endfor
         endif
endfunction

