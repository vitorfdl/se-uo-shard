//
// event IDs to be returned in 'type' member of wait_for_event
// result.
//
// See documentation for details.
//
// DO NOT MODIFY THIS FILE!  These are system events.  New core distributions
// will include new versions of this file.  User events should be placed in 
// eventid.inc, and must have more than one bit set (ie not be any of the
// values found in this file)
//
// Last modification: 11/16/2000
//

const EVID_HERDING     := 0x1111;
const EVID_PEACEMADE   := 0x90000000;
const EVID_TAUNT       := 0x2222;
const EVID_FLEE        := 0x3333;
const EVID_COMBAT_MOVE := 0x4444;
const EVID_DOOBJECTIVE := 0x5555;

Const EVID_ALL_FOLLOW_CMD := 0x11;
Const EVID_ALL_ATTACK_CMD := 0x12;
Const EVID_ALL_GUARD_CMD  := 0x13;

Const EVID_SPEECH          := 0x0001;
Const EVID_ENGAGED         := 0x0002;
Const EVID_DISENGAGED      := 0x0004;
Const EVID_DAMAGED         := 0x0008;
Const EVID_ENTEREDAREA     := 0x0010;
Const EVID_LEFTAREA        := 0x0020;
Const EVID_OPPONENT_MOVED  := 0x0040;
Const EVID_HOSTILE_MOVED   := 0x0080; // not implemented
Const EVID_MERCHANT_BOUGHT := 0x0100;
Const EVID_MERCHANT_SOLD   := 0x0200;
Const EVID_ITEM_GIVEN      := 0x0400;
Const EVID_DOUBLECLICKED   := 0x0800;


const SYSEVENT_SPEECH            := 0x00000001;

const SYSEVENT_ENGAGED           := 0x00000002;
const SYSEVENT_DISENGAGED        := 0x00000004;

const SYSEVENT_DAMAGED           := 0x00000008;

const SYSEVENT_ENTEREDAREA       := 0x00000010;
const SYSEVENT_LEFTAREA          := 0x00000020;

const SYSEVENT_OPPONENT_MOVED    := 0x00000040;

const SYSEVENT_HOSTILE_MOVED     := 0x00000080; // not implemented

const SYSEVENT_MERCHANT_BOUGHT   := 0x00000100;
const SYSEVENT_MERCHANT_SOLD     := 0x00000200;

const SYSEVENT_ITEM_GIVEN        := 0x00000400;

const SYSEVENT_DOUBLECLICKED     := 0x00000800;

const SYSEVENT_GHOST_SPEECH      := 0x00001000;

const SYSEVENT_GONE_CRIMINAL     := 0x00002000;

const SYSEVENT_RESERVED_00002000 := 0x00002000;
const SYSEVENT_RESERVED_00004000 := 0x00004000;
const SYSEVENT_RESERVED_00008000 := 0x00008000;

const SYSEVENT_RESERVED_00010000 := 0x00010000;
const SYSEVENT_RESERVED_00020000 := 0x00020000;
const SYSEVENT_RESERVED_00040000 := 0x00040000;
const SYSEVENT_RESERVED_00080000 := 0x00080000;
const SYSEVENT_RESERVED_00100000 := 0x00100000;
const SYSEVENT_RESERVED_00200000 := 0x00200000;
const SYSEVENT_RESERVED_00400000 := 0x00400000;
const SYSEVENT_RESERVED_00800000 := 0x00800000;
const SYSEVENT_RESERVED_01000000 := 0x01000000;
const SYSEVENT_RESERVED_02000000 := 0x02000000;
const SYSEVENT_RESERVED_04000000 := 0x04000000;
const SYSEVENT_RESERVED_08000000 := 0x08000000;
const SYSEVENT_RESERVED_10000000 := 0x10000000;
const SYSEVENT_RESERVED_20000000 := 0x20000000;
const SYSEVENT_RESERVED_40000000 := 0x40000000;
const SYSEVENT_RESERVED_80000000 := 0x80000000;
