use uo;

//include "include/bank";
include ":datafile:include/datafile";
include ":unicos:item_template";

const NO_CREATE = 0x0;

function GetMessengersDataFile()
	var data_file := DFOpenDataFile(":merchants:messengers", DF_CREATE);
	return data_file;
endfunction

function GetMessengerDataElem(elem_name, is_create := DF_CREATE)
   elem_name := String(elem_name);
	var data_file := GetMessengersDataFile();
	var data_elem := DFFindElement(data_file, elem_name, is_create);
	
	return data_elem;
endfunction

function CreateMailBox(serial, name, mailbox_serial := 0) 
   var mail_elem := GetMessengerDataElem(serial);
   if (!mail_elem) return 0; endif

   mail_elem.SetProp( "messages", array{});
   mail_elem.SetProp( "name", name);
   if (mailbox_serial) 
      mail_elem.SetProp( "mailbox", mailbox_serial);
   endif

   if (!SystemFindObjectBySerial(serial))
      mail_elem.SetProp( "is_npc", 1);
   endif

   return 1;
endfunction

function ListAllContacts()
   var data_file := GetMessengersDataFile();
   return data_file.keys();
endfunction

function StoreMessage(serial, from_serial, item, is_urgent := 0)
   var template := CreateItemTemplate(item);
   var mail_elem := GetMessengerDataElem(serial, NO_CREATE);
   if (!mail_elem) return 0; endif

   new_message := struct{
      "item" := template,
      "from" := from_serial,
      "urgent" := is_urgent
   };

   var messages := mail_elem.GetProp("messages");
   if (!messages) messages := array{}; endif
   messages.append(new_message);

   mail_elem.SetProp("messages", messages);
   return 1;
endfunction

function GetMessages(serial)
   var mail_elem := GetMessengerDataElem(serial, NO_CREATE);
   if (!mail_elem) return 0; endif

   var messages := mail_elem.GetProp("messages");
   if (!messages) messages := array{}; endif
   
   return messages;
endfunction

function UpdateMessages(serial, message_array)
   var mail_elem := GetMessengerDataElem(serial, NO_CREATE);
   if (!mail_elem) return 0; endif

   mail_elem.SetProp("messages", message_array);
endfunction

function GetUrgentMessages(serial)
   var mail_elem := GetMessengerDataElem(serial, NO_CREATE);
   if (!mail_elem) return 0; endif

   var messages := mail_elem.GetProp("messages");
   if (!messages) messages := array{}; endif

   var urgent_msgs := array{};
   foreach msg in messages
      if (msg.urgent)
         msg.index := _msg_iter_;
         urgent_msgs.append(msg);
      endif
      sleepms(2);
   endforeach

   if (!urgent_msgs.size()) return; endif

   return messages;
endfunction

function SendCrown(serial)
   var messages := GetUrgentMessages(serial);
   if (messages.size() < 1) return; endif

   var player := SystemFindObjectBySerial(serial);
   if (!player) return; endif

   if (IsInDungeon(player)) return; endif

   var npc := CreateNPCFromTemplate(":ghaia:aguia", player.x, player.y, player.z, 0, player.realm);
   if (!npc) return; endif
   npc.name := "Aguia treinada";
   npc.color := 0;
   npc.script := ":ghaia:orb_brain";
   SetObjProperty(npc, "msg_player", serial);
   PlaySoundEffectPrivate( npc, 0x90, player );
   RestartScript(npc);
   SendSysMessageEx(player, "Dê duplo clique na águia para receber suas cartas", SSM_INFO);
   TS_StartTimer(npc, "dynamickill", 400);
   return 1;
endfunction