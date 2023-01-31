/*
 * Author: ikki Chung
 * Date: 21 Sep 2016
 * Description:
 *      SMIMS VILS RTLS Solutions event time line
 **/

var firedEvents_count = 30;
var firedEvents_offset = 0;
var firedEvents_start_date = '';
var gl_allevents = [];
var gl_ui_firedEvents_offset = 0;

// var gl_employee_info = [];
var gl_user_info = [];

var gl_EditMode = false;

// load_employee_info();
load_user_info();

function load_user_info()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'userinfo=1'+'&project_id='+project_id, function(data)
    {
        var index = 0;
        $.each(data, function(empid, value)
        {
            var user = [];
            user.userid = parseInt(value.userid);
            user.account = value.account;
            user.empid = parseInt(value.userid);
            user.cname = value.name;
            user.orgName = value.unit;
            //user.jobTitle = value.title;
            user.gender = value.gender;

            gl_user_info.push(user);
        });
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

// To make Pace works on Ajax calls
//$(document).ajaxStart(function() { Pace.restart(); });

$('.ajax').click(function()
{

    //=============================
    load_events_from_server();
    //=============================
    //load_events_from_testdata();
    // load_events_from_testdata_1();
    // load_events_from_testdata_2();
    // load_events_from_testdata_3();
    // load_events_from_testdata_4();
    // load_events_from_testdata_5();
    // load_events_from_testdata_6();
    // load_events_from_testdata_7();
    // //
    // var gl_allevents_rev = gl_allevents.reverse();
    // var merge_events = merge_same_event(gl_allevents_rev);
    // var pair_events = pair_event(merge_events);
    // var same_tagaction_events = merge_same_tagaction_event(pair_events);
    // var same_tagaction_events_rev = same_tagaction_events.reverse();
    // show_pairevent_timeline(same_tagaction_events_rev);
    // =============================

});

function load_events_from_server()
{
    var targetURL = gl_target_server + '/php/eventfire.php?project_id='+project_id+'&prettystring=1&markasread=1&count='+firedEvents_count+'&offset='+firedEvents_offset;

    var new_event_count = 0;
    $.getJSON(targetURL,
    function(data)
    {
        $.each(data, function(itemindex, event)
        {
            new_event_count++;
            gl_allevents.push(event);
        });
    })
    .success(function() {

        //=========================================================
        var gl_allevents_orig = gl_allevents.slice();
        var gl_allevents_rev = gl_allevents_orig.reverse();
        var merge_events = merge_same_event(gl_allevents_rev);
        var pair_events = pair_event(merge_events);
        var same_tagaction_events = merge_same_tagaction_event(pair_events);
        var same_tagaction_events_rev = same_tagaction_events.reverse();
        show_pairevent_timeline(same_tagaction_events_rev);
        //=========================================================
        // var merge_events = merge_same_event(gl_allevents);
        // var pair_events = pair_event(merge_events);
        // show_pairevent_timeline(pair_events);
        //=========================================================
        //show_event_timeline(gl_allevents);
        //=========================================================

        // console.log("load_events_from_server() new_event_count:"+ new_event_count);

        if (new_event_count == 0)
        {
            $("#AjaxLoadMode").prop('disabled', true);
            $("#AjaxLoadMode").prop('class', 'btn btn-lrg ajax');
            $("#AjaxLoadMode").html('No more data.');

            $("#AjaxLoadModeicon").hide();
        }

        if (new_event_count > 0)
        {
            firedEvents_offset += new_event_count;
            firedEvents_offset++;
        }
    });
}


function merge_same_event(inputevents)
{
    var merge_events = [];
    var working_events = inputevents;

    // console.log("merge_same_event() inputevents.length:"+inputevents.length);

    var index;
    for(index=0; index < working_events.length; index++)
    {
        var event = working_events[index];
        var eventid = event.eventjson.eventid;
        if ( eventid == undefined )
        {
            // inspectionGroup event
            var mergeevent = {};
            mergeevent.type = 'none';
            mergeevent.events = [];
            mergeevent.events.push(event);
            merge_events.push(mergeevent);
        }
        else
        if ( eventid === '9999' || eventid === '8888' || eventid === '111111')
        {
            var cur_event_nodeid = event.eventjson.nodeid;
            var cur_event_action = event.eventjson.action;
            var cur_event_locatorid = event.eventjson.locatorid;
            var cur_event_datetime = event.datetime;

            if (event.eventjson.nodeid == undefined && event.eventjson.locatorid == undefined)
            {
                cur_event_nodeid = event.eventjson.cardid;
                cur_event_locatorid = event.eventjson.cardid;
                //console.log("merge_same_event() 1 cardid:" + event.eventjson.cardid);
                if (event.eventjson.loramacaddress != undefined)
                    cur_event_locatorid = event.eventjson.loramacaddress;
            }

            var mergeevent = {};
            mergeevent.type = eventid;
            mergeevent.events = [];
            mergeevent.events.push(event);

            // console.log("merge_same_event() cur_event_nodeid:"+cur_event_nodeid+" cur_event_action:"+cur_event_action +" datetime:"+cur_event_datetime);

            for(var checkIdx = index+1; checkIdx < working_events.length;)
            {
                var check_event = working_events[checkIdx];
                var check_eventid = check_event.eventjson.eventid;
                if ( check_eventid == undefined )
                {
                    checkIdx++;
                    continue;
                }

                // check same event
                var check_event_nodeid = check_event.eventjson.nodeid;
                var check_event_action = check_event.eventjson.action;
                var check_event_locatorid = check_event.eventjson.locatorid;
                var check_event_datetime = check_event.datetime;

                if (check_event.eventjson.nodeid == undefined && check_event.eventjson.locatorid == undefined)
                {
                    check_event_nodeid = check_event.eventjson.cardid;
                    check_event_locatorid = check_event.eventjson.cardid;
                    if (check_event.eventjson.loramacaddress != undefined)
                        check_event_locatorid = check_event.eventjson.loramacaddress;
                    //console.log("merge_same_event() 2 cardid:" + check_event.eventjson.cardid);
                }

                // console.log("merge_same_event() cur_event_nodeid:"+cur_event_nodeid+" cur_event_action:"+cur_event_action+" cur_event_locatorid:"+cur_event_locatorid+" cur_event_datetime:"+cur_event_datetime);
                // console.log("merge_same_event() check_event_nodeid:"+check_event_nodeid+" check_event_action:"+check_event_action+" check_event_locatorid:"+check_event_locatorid+" check_event_datetime:"+check_event_datetime);

                if ( cur_event_nodeid === check_event_nodeid &&
                    cur_event_action === check_event_action &&
                    cur_event_locatorid === check_event_locatorid )
                {
                    var cur_datetime = moment(cur_event_datetime, 'YYYY-MM-DD HH:mm:ss');
                    var check_datetime = moment(check_event_datetime, 'YYYY-MM-DD HH:mm:ss');
                    var ms = check_datetime.diff(cur_datetime);
                    var diffSeconds = moment.duration(ms) / 1000; // ms to seconds

                    var timeout = 61; // one minites
                    if ( cur_event_action === 'Cancel' )
                       timeout = 90;
                    if ( cur_event_action === 'CallOut' )
                       timeout = 300;

                    if ( eventid === '9999' )
                    {
                        var cur_event_areaid = event.eventjson.areaid;
                        var check_event_areaid = check_event.eventjson.areaid;
                        if ( cur_event_areaid != check_event_areaid )
                        {
                            // keep different event
                            break;
                        }
                    }
                    if (diffSeconds < timeout)
                    {
                        //console.log("merge_same_event() check_event_nodeid:"+check_event_nodeid+" check_event_action:"+check_event_action +" datetime:"+check_event_datetime);
                        // console.log("merge_same_event() check_event_nodeid:"+check_event_nodeid+" check_event_locatorid:"+check_event_locatorid+" check_event_action:"+check_event_action +" datetime:"+check_event_datetime);

                        // keep as same event
                        mergeevent.events = addto_merageevent_list(mergeevent.events, check_event);
                        //mergeevent.events.push(check_event);

                        // remove from list
                        working_events.splice(checkIdx, 1);
                    }
                    else
                    {
                        // keep different event
                        break;
                    }
                }
                else
                if (cur_event_action != check_event_action &&
                    cur_event_locatorid === check_event_locatorid)
                {
                    // console.log("merge_same_event() check_event_nodeid:"+check_event_nodeid+" check_event_action:"+check_event_action +" datetime:"+check_event_datetime +" same locator but different action");
                    // same locator but different action
                    // keep different event
                    //checkIdx++;
                    break;
                }
                else
                {
                    checkIdx++;
                }
            }

            // output event
            merge_events.push(mergeevent);
        }
        else
        {
            var mergeevent = {};
            mergeevent.type = 'none';
            mergeevent.events = [];
            mergeevent.events.push(event);
            merge_events.push(mergeevent);
        }
    }

    return merge_events;
}

function addto_merageevent_list(events, check_event)
{
    // check if same locatorid, just update firstdatetime
    var bFound = false;
    for(var index=0; index < events.length; index++)
    {
        var event = events[index];

        if ( event.eventjson.locatorid === check_event.eventjson.locatorid)
        {
            event.lastdatetime = check_event.datetime;
            bFound = true;
            // break;
        }
        if (event.eventjson.locatorid == undefined)
        {
            if ( event.eventjson.cardid === check_event.eventjson.cardid)
            {
                event.lastdatetime = check_event.datetime;
                bFound = true;
                // break;
            }
        }
    }
    if ( bFound == false )
    {
        events.push(check_event);
    }

    return events;
}

function pair_event(inputevents)
{
    var pair_events = [];
    var working_events = inputevents;

    // console.log("pair_event() inputevents.length:"+inputevents.length);

    var index;
    for(index=0; index < working_events.length; index++)
    {
        // var event = working_events[index];
        //console.log("pair_event()");
        // console.log("pair_event() working_events.length:"+working_events.length +", index:"+index);

        var mergeevent = working_events[index];
        var event = mergeevent.events[0];

        // console.log("pair_event() index:"+index+" mergeevent.events.length:"+ mergeevent.events.length);

        var eventid = event.eventjson.eventid;
        if ( eventid == undefined )
        {
            // inspectionGroup event
            //console.log("pair_event() index:"+index+" inspectionGroup event");

            var pairevent = {};
            pairevent.type = 'single';
            pairevent.events1 = [];
            pairevent.events2 = [];
            pairevent.events1.push(event);

            pair_events.push(pairevent);
        }
        else
        if ( eventid === '9999' || eventid === '8888' || eventid === '119999' || eventid === '229999')
        {
            var cur_event_nodeid = event.eventjson.nodeid;
            var cur_event_action = event.eventjson.action;
            var cur_event_locatorid = event.eventjson.locatorid;
            var cur_event_datetime = event.datetime;
            var cur_event_lastdatetime = event.lastdatetime;

            // console.log("pair_event() index:"+index+" nodeid:"+ cur_event_nodeid+" action:"+ cur_event_action+" datetime:"+ cur_event_datetime+" lastdatetime:"+ cur_event_lastdatetime);

            // for(var event1idx = 0; event1idx < mergeevent.events.length; event1idx++)
            // {
            //     var event1 = mergeevent.events[event1idx];
            //     console.log("pair_event() event1idx:"+event1idx+" action:"+ event1.eventjson.action +" locatorid:" + event1.eventjson.locatorid+" datetime:"+ event1.datetime+" lastdatetime:"+ event1.lastdatetime);
            // }

            //console.log("pair_event() "+cur_event_nodeid+" "+cur_event_action+" "+cur_event_locatorid+" "+cur_event_datetime+" "+cur_event_firstdatetime);

            //if ( cur_event_action === "Cancel")
            if ( cur_event_action === "Alert" || cur_event_action === "AlertTagNotMoving" || cur_event_action === "AlertLayDown" || cur_event_action === "CallOut")
            {
                var pairevent = {};
                pairevent.type = 'pair';
                pairevent.events1 = [];
                pairevent.events2 = [];

                // find Cancel
                var Found = false;
                var checkIdx = index+1;
                for(checkIdx = index+1; checkIdx < working_events.length;checkIdx++)
                {
                    // var check_event = working_events[checkIdx];

                    var check_mergeevent = working_events[checkIdx];
                    var check_event = check_mergeevent.events[0];
                    var check_eventid = check_event.eventjson.eventid;
                    if ( eventid == undefined )
                    {
                        continue;
                    }

                    // console.log("pair_event() checkIdx:"+checkIdx+" check_mergeevent.events.length:"+ check_mergeevent.events.length);

                    // check alert event
                    var check_event_nodeid = check_event.eventjson.nodeid;
                    var check_event_action = check_event.eventjson.action;
                    var check_event_locatorid = check_event.eventjson.locatorid;
                    var check_event_datetime = check_event.datetime;

                    if ( "Cancel" === check_event_action )
                    {
                        //
                        // find same locator action
                        //
                        for(var event1idx = 0; event1idx < mergeevent.events.length; event1idx++)
                        {
                            var event1 = mergeevent.events[event1idx];
                            var event1_locatorid = event1.eventjson.locatorid;

                            for(var checkeventidx = 0; checkeventidx < check_mergeevent.events.length; checkeventidx++)
                            {
                                var check_event1 = check_mergeevent.events[checkeventidx];
                                var check_event1_locatorid = check_event1.eventjson.locatorid;

                                if ( event1_locatorid === check_event1_locatorid )
                                {
                                    // console.log("pair_event() checkeventidx:"+checkeventidx+" action:"+ check_event1.eventjson.action +" locatorid:" + check_event1.eventjson.locatorid+" datetime:"+ check_event1.datetime+" lastdatetime:"+ check_event1.lastdatetime);

                                    // check datetime
                                    var event1_datetime = moment(event1.datetime, 'YYYY-MM-DD HH:mm:ss');
                                    var check_event1_datetime = moment(check_event1.datetime, 'YYYY-MM-DD HH:mm:ss');

                                    var ms = check_event1_datetime.diff(event1_datetime);
                                    var diffSeconds = moment.duration(ms) / 1000; // ms to seconds

                                    if (diffSeconds < 600)
                                    {
                                        Found = true;
                                        // console.log("pair_event() Found!!!!");
                                    }
                                    break;
                                }
                                if ( Found )
                                {
                                    break;
                                }
                            }
                            if ( Found )
                            {
                                break;
                            }
                        }
                        if ( Found )
                        {
                            // output event
                            Array.prototype.push.apply(pairevent.events1, check_mergeevent.events);
                            Array.prototype.push.apply(pairevent.events2, mergeevent.events);
                            break;
                        }
                    }
                }

                if (Found == true)
                {
                    // console.log("pair_event() skip next one index:"+index +" -> " + checkIdx);
                    pair_events.push(pairevent);
                    // index++;// skip next one
                    index = checkIdx;
                }
                else
                if (Found == false)
                {
                    // output event
                    var pairevent = {};
                    pairevent.type = 'single';
                    pairevent.events1 = [];
                    pairevent.events2 = [];
                    pairevent.events1.push(event);
                    pair_events.push(pairevent);
                }
            }
            else
            {
                //console.log("pair_event() index:"+index+" eventid:"+eventid);

                // output event
                var pairevent = {};
                pairevent.type = 'single';
                pairevent.events1 = [];
                pairevent.events2 = [];
                pairevent.events1.push(event);
                pair_events.push(pairevent);
            }
        }
        else
        {
            // output event
            var pairevent = {};
            pairevent.type = 'single';
            pairevent.events1 = [];
            pairevent.events2 = [];
            pairevent.events1.push(event);
            pair_events.push(pairevent);
        }
    }

    return pair_events;
}

function merge_same_tagaction_event(pair_events)
{
    var same_tagaction_events = [];
    var working_events = pair_events;

    var index;
    for(index=0; index < working_events.length; index++)
    {
        //console.log("pair_event() index:"+index+" working_events.length:"+working_events.length+" pair_events.length:"+pair_events.length);

        var cur_event = working_events[index];
        if ( cur_event.type === 'single' )
        {
            var cur_event_event1 = cur_event.events1[0];

            var cur_event_nodeid = cur_event_event1.eventjson.nodeid;
            var cur_event_action = cur_event_event1.eventjson.action;
            var cur_event_datetime = cur_event_event1.datetime;

            if (cur_event_event1.eventjson.nodeid == undefined)
            {
                cur_event_nodeid = cur_event_event1.eventjson.cardid;
            }

            for(var checkIdx = index+1; checkIdx < working_events.length;checkIdx++)
            {
                var check_event = working_events[checkIdx];
                if ( check_event.type === 'single' )
                {
                    var check_event_event1 = check_event.events1[0];

                    var check_event_nodeid = check_event_event1.eventjson.nodeid;
                    var check_event_action = check_event_event1.eventjson.action;
                    var check_event_datetime = check_event_event1.datetime;

                    if (check_event_event1.eventjson.nodeid == undefined)
                    {
                        check_event_nodeid = check_event_event1.eventjson.cardid;
                    }

                    var check_eventid = check_event_event1.eventjson.eventid;
                    if ( check_eventid == undefined )
                    {
                        continue;
                    }

                    // check event in one minute
                    var cur_datetime = moment(cur_event_datetime, 'YYYY-MM-DD HH:mm:ss');
                    var check_datetime = moment(check_event_datetime, 'YYYY-MM-DD HH:mm:ss');
                    // var ms = cur_datetime.diff(check_datetime);
                    var ms = check_datetime.diff(cur_datetime);
                    var diffSeconds = moment.duration(ms) / 1000; // ms to seconds

                    if ( diffSeconds > 60 )
                    {
                        break;
                    }

                    // found same node
                    if ( cur_event_nodeid === check_event_nodeid )
                    {
                        // change current event to sametag
                        cur_event.type = 'sametag';
                        // create event array
                        //if (cur_event.events1 == undefined )
                        //    cur_event.events1 = [];

                        cur_event.events1.push(check_event_event1);

                        //
                        // remove check_event
                        //
                        // remove from list
                        working_events.splice(checkIdx, 1);
                    }
                }
            }


            same_tagaction_events.push(cur_event);
        }
        else
        {
            // output event
            same_tagaction_events.push(cur_event);
        }
    }

    return same_tagaction_events;
}

function show_pairevent_timeline(showevents)
{
    gl_ui_firedEvents_offset = 0;

    var index;
    for(index=0;index<showevents.length;index++)
    {
        var pairevent = showevents[index];

        if ( pairevent.type === 'single' )
        {
            var new_html = create_single_event_html(pairevent.events1[0], gl_ui_firedEvents_offset+1);

            $('.ajax-content-'+gl_ui_firedEvents_offset).html(new_html);
            gl_ui_firedEvents_offset += 1;
        }
        else
        if ( pairevent.type === 'pair' )
        {
            // console.log("show_pairevent_timeline() pair ");
            var new_html = create_pair_event_html(pairevent.events1, pairevent.events2, gl_ui_firedEvents_offset+1);

            $('.ajax-content-'+gl_ui_firedEvents_offset).html(new_html);
            gl_ui_firedEvents_offset += 1;
        }
        else
        if ( pairevent.type === 'sametag' )
        {
            // console.log("show_pairevent_timeline() sametag ");
            var events1_rev = pairevent.events1.reverse();
            var new_html = create_sametag_event_html(events1_rev, gl_ui_firedEvents_offset+1);

            $('.ajax-content-'+gl_ui_firedEvents_offset).html(new_html);
            gl_ui_firedEvents_offset += 1;
        }
    }
}

function create_single_event_html(event, offset)
{
    var prettyjson = event.prettyjson;
    var event_eventjson = event.eventjson;
    var event_datetime = event.datetime;
    var event_lastdatetime = event.lastdatetime;

   // console.log("create_single_event_html() nodeid:"+event_eventjson.nodeid + " ,action:"+event_eventjson.action +" ,locatorid:"+event_eventjson.locatorid +", datetime:"+event.datetime);

    var new_html = '<ul class="timeline">';
    var cur_date = moment(event_datetime).format("YYYY MMM Do");
    if ( firedEvents_start_date != cur_date )
    {
        firedEvents_start_date = cur_date;
        //
        // add time-label
        //
        new_html += '<li class="time-label"><span class="bg-red">'+cur_date+'</span></li>';
    }

    //
    // event body
    //
    if (event_eventjson.action == 'Enter')
        new_html += '<li><i class="fa fa-sign-in bg-aqua"></i>';
    else
    if (event_eventjson.action == 'Leave')
        new_html += '<li><i class="fa fa-share-square-o bg-yellow-active"></i>';
    else
    if (event_eventjson.action == 'Stay')
        new_html += '<li><i class="fa fa-street-view bg-aqua-active"></i>';
    else
    if (event_eventjson.action == 'Alert' || event_eventjson.action == 'AlertTagNotMoving' || event_eventjson.action == 'AlertLayDown')
        new_html += '<li><i class="fa fa-bell-o bg-red"></i>';
    else
    if (event_eventjson.action == 'Cancel')
        new_html += '<li><i class="fa fa-bell-slash-o bg-blue"></i>';
    else
    if (event_eventjson.action == 'CallOut')
        new_html += '<li><i class="fa fa-hand-paper-o bg-green"></i>';
    else
    if (event_eventjson.action == 'PowerOff')
        new_html += '<li><i class="fa fa-flash bg-red"></i>';
    else
    if (event_eventjson.action == 'PowerOn')
        new_html += '<li><i class="fa fa-flash bg-green"></i>';
    else
    if (event_eventjson.action == 'OpenDoor')
        new_html += '<li><i class="fa fa-unlock bg-red"></i>';
    else
    if (event_eventjson.action == 'CloseDoor')
        new_html += '<li><i class="fa fa-lock bg-green"></i>';
    else
    if (event_eventjson.action == 'iCoverOpenDoor')
        new_html += '<li><i class="fa fa-unlock bg-red"></i>';
    else
    if (event_eventjson.action == 'iCoverCloseDoor')
        new_html += '<li><i class="fa fa-lock bg-green"></i>';
    else
    if (event_eventjson.action == 'EnterDoor')
        new_html += '<li><i class="fa fa-sign-in bg-red"></i>';
    else
    if (event_eventjson.action == 'ExitDoor')
        new_html += '<li><i class="fa fa-sign-out bg-green"></i>';
    else
    if (event_eventjson.action == 'SensorAlert')
        new_html += '<li><i class="fa fa-rss bg-red"></i>';
    else
    if (event_eventjson.action == 'unknowncard')
        new_html += '<li><i class="fa fa-warning bg-red"></i>';
    else
    if (event_eventjson.action == 'BodyInfoAlertNormal')
        new_html += '<li><i class="fa fa-street-view bg-green"></i>';
    else
    if (event_eventjson.action == 'BodyInfoAlertWarning')
        new_html += '<li><i class="fa fa-street-view bg-yellow"></i>';
    else
    if (event_eventjson.action == 'BodyInfoAlertDanger')
        new_html += '<li><i class="fa fa-street-view bg-red"></i>';
    else
        new_html += '<li><i class="fa fa-street-view bg-yellow"></i>';


    new_html += '<div class="timeline-item">';

    new_html += '<span class="time"><i class="fa fa-clock-o"></i>&nbsp;'+moment(event_datetime).format("hh:mm:ss A")+'</span>';
    if (event_lastdatetime != undefined)
    {
        new_html += '<span class="time"><i class="fa fa-clock-o"></i>&nbsp;'+moment(event_lastdatetime).format("hh:mm:ss A")+'</span>';
    }

    if (prettyjson.type == 'inspectionpoint')
    {
        new_html += '<a href="javascript:;" onclick="markclick(\'tag\', 0);return false;">'+prettyjson.tagname+'&nbsp;&nbsp;</a></h3>';
        new_html += '<div class="timeline-body">';
        new_html += convert_action_name(prettyjson.actionname)+'&nbsp;</br>';
        new_html += '巡檢點:'+ prettyjson.inspectionpointname+'&nbsp;';
    }
    else
    if (prettyjson.type == 'inspectionpointtravel')
    {
        if (prettyjson.tagname.length > 0)
            new_html += '<a href="javascript:;" onclick="markclick(\'tag\', 0);return false;">'+prettyjson.tagname+'&nbsp;&nbsp;</a></h3>';
        new_html += '<div class="timeline-body">';
        new_html += convert_action_name(prettyjson.actionname)+'&nbsp;</br>';
        new_html += '巡檢點:';
        if ( prettyjson.startinspectionpointname != null && prettyjson.endinspectionpointname != null)
        {
            new_html += prettyjson.startinspectionpointname;
            new_html += '&nbsp;->';
            new_html += prettyjson.endinspectionpointname;
        }
        else
        if ( prettyjson.startinspectionpointname != null )
        {
            new_html += prettyjson.startinspectionpointname;
        }
        else
        if ( prettyjson.endinspectionpointname != null )
        {
            new_html += prettyjson.endinspectionpointname;
        }
    }
    else
    if (prettyjson.type == 'inspectionGroup')
    {
        new_html += '<a href="javascript:;" onclick="markclick(\'tag\', 0);return false;">'+prettyjson.groupname+'&nbsp;&nbsp;</a></h3>';
        new_html += '<div class="timeline-body">';
        new_html += convert_action_name(prettyjson.actionname)+'&nbsp;</br>';
    }
    else
    if (prettyjson.type == 'rfidcard')
    {
        new_html += '<a href="javascript:;" onclick="copyTextToClipboard(\''+ prettyjson.cardid +'\', \''+ event_datetime +'\');return false;">'+prettyjson.cardid+'&nbsp;&nbsp;</a></h3>';
        new_html += '<span id="cardid_copy_alert_'+prettyjson.cardid+'_'+ event_datetime +' "></span>';
        new_html += '<div class="timeline-body">';
        new_html += prettyjson.locatorname + '&nbsp;';
        new_html += convert_action_name(prettyjson.actionname)+'&nbsp;</br>';
    }
    else
    if (prettyjson.type == 'nfccard')
    {
        new_html += '<a href="javascript:;" onclick="copyTextToClipboard(\''+ prettyjson.cardid +'\', \''+ event_datetime +'\');return false;">'+prettyjson.cardid+'&nbsp;&nbsp;</a></h3>';
        new_html += '<span id="cardid_copy_alert_'+prettyjson.cardid+'_'+ event_datetime +'"></span>';
        new_html += '<div class="timeline-body">';
        new_html += prettyjson.locatorname + '&nbsp;';
        new_html += convert_action_name(prettyjson.actionname)+'&nbsp;</br>';
    }
    else
    {
        new_html += '<a href="javascript:;" onclick="markclick(\'tag\', 0);return false;">'+ prettyjson.nodename+'&nbsp;&nbsp;</a></h3>';
        new_html += '<div class="timeline-body">';

        if( prettyjson.locatorname.length > 0 && prettyjson.locatorname != prettyjson.nodename)
        {
            new_html += '在'+prettyjson.locatorname;
        }

        if ( event_eventjson.lat != undefined )
        {
            new_html += '位置'+event_eventjson.lat +','+event_eventjson.lng+'&nbsp;';
            new_html += '<a href="javascript:;" onclick="open_gmap('+event_eventjson.lat +','+event_eventjson.lng+');return false;"><i class="fa fa-map-marker"></i></a>';
            new_html += '&nbsp;</br>';
        }

        new_html += convert_action_name(prettyjson.actionname)+'&nbsp;';
        new_html += prettyjson.areaname+'&nbsp;';

        if ( !(event_eventjson.issuetime === "0") )
        {
            new_html += convert_IssueTime_name(prettyjson.timename)+'&nbsp;';
        }
    }

    new_html += '</div>'; // timeline-body

    if ( event_eventjson.eventid === '9999')
    {
        new_html += create_extra_info(event);
    }

    new_html += '</div>'; // timeline-item
    new_html += '</li>';

    //
    // add new ajax-content
    //
    new_html += '</ul><div class="ajax-content-'+(offset)+'"></div>';
    return new_html;
}

function create_pair_event_html(events_cancel, events_alert, offset)
{
    var event_1 = events_alert[0];

    var event_1_prettyjson = event_1.prettyjson;
    var event_1_eventjson = event_1.eventjson;
    var event_1_datetime = event_1.datetime;
    var event_1_lastdatetime = event_1.lastdatetime;

    // console.log("create_pair_event_html() nodeid:"+event_1.nodeid + " ,action:"+event_1.action +" ,locatorid:"+event_1.locatorid +", datetime:"+event_1.datetime);

    var new_html = '<ul class="timeline">';
    var cur_date = moment(event_1_datetime).format("YYYY MMM Do");
    if ( firedEvents_start_date != cur_date )
    {
        firedEvents_start_date = cur_date;
        //
        // add time-label
        //
        new_html += '<li class="time-label"><span class="bg-red">'+cur_date+'</span></li>';
    }

    var show_name = event_1_prettyjson.nodename;
    var add_extra = true;

    //
    // event body
    //
    if (event_1_eventjson.action == 'Alert' || event_eventjson.action == 'AlertTagNotMoving' || event_eventjson.action == 'AlertLayDown')
        new_html += '<li><i class="fa fa-bell-o bg-red"></i>';
    else
    if (event_1_eventjson.action == 'CallOut')
        new_html += '<li><i class="fa fa-hand-paper-o bg-green"></i>';
    else
    if (event_1_eventjson.action == 'PowerOff')
        new_html += '<li><i class="fa fa-flash bg-red"></i>';
    else
    if (event_1_eventjson.action == 'PowerOn')
        new_html += '<li><i class="fa fa-flash bg-green"></i>';
    else
    if (event_1_eventjson.action == 'OpenDoor')
        new_html += '<li><i class="fa fa-unlock bg-red"></i>';
    else
    if (event_1_eventjson.action == 'CloseDoor')
        new_html += '<li><i class="fa fa-lock bg-green"></i>';
    else
    if (event_1_eventjson.action == 'iCoverOpenDoor')
        new_html += '<li><i class="fa fa-unlock bg-red"></i>';
    else
    if (event_1_eventjson.action == 'iCoverCloseDoor')
        new_html += '<li><i class="fa fa-lock bg-green"></i>';
    else
    if (event_1_eventjson.action == 'EnterDoor')
        new_html += '<li><i class="fa fa-sign-in bg-red"></i>';
    else
    if (event_1_eventjson.action == 'ExitDoor')
        new_html += '<li><i class="fa fa-sign-out bg-green"></i>';
    else
    if (event_1_eventjson.action == 'SensorAlert')
        new_html += '<li><i class="fa fa-rss bg-red"></i>';
    else
    if (event_1_eventjson.action == 'unknowncard')
    {
        show_name = event_1_eventjson.cardid;
        add_extra = false;
        new_html += '<li><i class="fa fa-warning bg-red"></i>';
    }
    else
        new_html += '<li><i class="fa fa-street-view bg-yellow"></i>';

    //============================================
    // event
    new_html += '<div class="timeline-item">';
    new_html += '<a href="javascript:;" onclick="markclick(\'tag\', 0);return false;">'+ show_name +'&nbsp;&nbsp;</a></h3>';
    new_html += '<div class="timeline-body">';
    new_html += '</div>'; // timeline-body
    new_html += '</div>'; // timeline-item
    //============================================

    //============================================
    // events_cancel
    var index;
    var event_cancel = null;
    for(index=0; index < events_cancel.length; index++)
    {
        var event = events_cancel[index];
        if ( event_cancel == null )
            event_cancel = event;
        new_html += create_event_html(event, false, event);
    }
    //============================================

    //============================================
    // events_alert
    var index;
    for(index=0; index < events_alert.length; index++)
    {
        var event = events_alert[index];

        if ( index == events_alert.length - 1 )
            new_html += create_event_html(event, add_extra, event_cancel);
        else
            new_html += create_event_html(event, false, event);
    }
    //============================================

    new_html += '</li>';

    //
    // add new ajax-content
    //
    new_html += '</ul><div class="ajax-content-'+(offset)+'"></div>';
    return new_html;

}

function create_sametag_event_html(events1, offset)
{
    var event_1 = events1[0];
    var event_1_prettyjson = event_1.prettyjson;
    var event_1_eventjson = event_1.eventjson;
    var event_1_datetime = event_1.datetime;
    var event_1_lastdatetime = event_1.lastdatetime;

    // console.log("create_sametag_event_html() nodeid:"+event_1_eventjson.nodeid + " ,action:"+event_1_eventjson.action +" ,locatorid:"+event_1_eventjson.locatorid +", datetime:"+event_1_datetime);

    var new_html = '<ul class="timeline">';
    var cur_date = moment(event_1_datetime).format("YYYY MMM Do");
    if ( firedEvents_start_date != cur_date )
    {
        firedEvents_start_date = cur_date;
        //
        // add time-label
        //
        new_html += '<li class="time-label"><span class="bg-red">'+cur_date+'</span></li>';
    }

    var show_name = event_1_prettyjson.nodename;
    var add_extra = true;

    //
    // event body
    //
    var event_first = event_1
    if (events1.length > 1)
        event_first = events1[events1.length - 1];

    // console.log("create_sametag_event_html() event_first.eventjson.action:"+event_first.eventjson.action+" event_1.eventjso.action:"+event_1.eventjson.action);

    if (event_first.eventjson.action === 'Alert' || event_first.action == 'AlertTagNotMoving' || event_first.action == 'AlertLayDown')
        new_html += '<li><i class="fa fa-bell-o bg-red"></i>';
    else
    if (event_first.eventjson.action === 'Cancel')
        new_html += '<li><i class="fa fa-bell-slash-o bg-blue"></i>';
    else
    if (event_first.eventjson.action === 'CallOut')
        new_html += '<li><i class="fa fa-hand-paper-o bg-green"></i>';
    else
    if (event_first.eventjson.action === 'PowerOff')
        new_html += '<li><i class="fa fa-flash bg-red"></i>';
    else
    if (event_first.eventjson.action === 'PowerOn')
        new_html += '<li><i class="fa fa-flash bg-green"></i>';
    else
    if (event_first.eventjson.action === 'OpenDoor')
        new_html += '<li><i class="fa fa-unlock bg-red"></i>';
    else
    if (event_first.eventjson.action === 'CloseDoor')
        new_html += '<li><i class="fa fa-lock bg-green"></i>';
    else
    if (event_first.eventjson.action === 'EnterDoor')
        new_html += '<li><i class="fa fa-sign-in bg-red"></i>';
    else
    if (event_first.eventjson.action === 'ExitDoor')
        new_html += '<li><i class="fa fa-sign-out bg-green"></i>';
    else
    if (event_first.eventjson.action === 'SensorAlert')
        new_html += '<li><i class="fa fa-rss bg-red"></i>';
    else
    if (event_first.eventjson.action == 'unknowncard')
    {
        show_name = event_1_eventjson.cardid;
        add_extra = false;
        new_html += '<li><i class="fa fa-warning bg-red"></i>';
    }
    else
        new_html += '<li><i class="fa fa-street-view bg-yellow"></i>';


    //============================================
    // node name
    new_html += '<div class="timeline-item">';
    new_html += '<a href="javascript:;" onclick="markclick(\'tag\', 0);return false;">'+ show_name+'&nbsp;&nbsp;</a></h3>';
    new_html += '<div class="timeline-body">';

    new_html += '</div>'; // timeline-body
    new_html += '</div>'; // timeline-item
    //============================================

    var index;
    for(index=0; index < events1.length; index++)
    {
        var event = events1[index];

        if (index == events1.length - 1)
            new_html += create_event_html(event, add_extra, event);
        else
            new_html += create_event_html(event, false, event);
    }
    new_html += '</li>';

    //
    // add new ajax-content
    //
    new_html += '</ul><div class="ajax-content-'+(offset)+'"></div>';
    return new_html;
}

function create_event_html(event, add_extra, extra_item)
{
    var event_prettyjson = event.prettyjson;
    var event_eventjson = event.eventjson;
    var event_datetime = event.datetime;
    var event_lastdatetime = event.lastdatetime ;

    var new_html = '<div class="timeline-item">';
    new_html += '<span class="time"><i class="fa fa-clock-o"></i>&nbsp;'+moment(event_datetime).format("hh:mm:ss A")+'</span>';
    if (event_lastdatetime != undefined)
    {
        new_html += '<span class="time"><i class="fa fa-clock-o"></i>&nbsp;'+moment(event_lastdatetime).format("hh:mm:ss A")+'</span>';
    }
    // new_html += '<a href="javascript:;" onclick="markclick(\'tag\', 0);return false;">'+ format_device_name(event_1_prettyjson.nodename)+'&nbsp;&nbsp;</a></h3>';
    new_html += '<div class="timeline-body">';

    if ( event_prettyjson.locatorname != undefined )
    {
        if( event_prettyjson.locatorname.length > 0)
        {
            if ( event_prettyjson.nodename != undefined )
            {
                if (event_prettyjson.nodename.length > 0 && event_prettyjson.locatorname != event_prettyjson.nodename)
                    new_html += event_prettyjson.nodename+'&nbsp;&nbsp;在'+event_prettyjson.locatorname;
                else
                    new_html += '在'+event_prettyjson.locatorname;
            }
            else
            {
                new_html += '在'+event_prettyjson.locatorname;
            }
        }

        // if( event_prettyjson.locatorname.length > 0 && event_prettyjson.locatorname != event_prettyjson.nodename)
        // {
        //     if (event_prettyjson.nodename.length > 0)
        //         new_html += event_prettyjson.nodename+'&nbsp;&nbsp;在'+event_prettyjson.locatorname;
        //     else
        //         new_html += '在'+event_prettyjson.locatorname;
        // }
    }

    new_html += convert_action_name(event_prettyjson.actionname)+'&nbsp;';
    if ( event_prettyjson.areaname != undefined )
    {
        new_html += event_prettyjson.areaname+'&nbsp;';
    }

    new_html += '</div>'; // timeline-body

    if (add_extra)
    {
        new_html += create_extra_info(extra_item);
    }

    new_html += '</div>'; // timeline-item

    return new_html;
}

function create_extra_info(event)
{
    var empid = event.empid;
    //var username = findEmployeeName(event.empid);
    var username = findUserName(event.empid);
    var comment = event.comment;

    var Assigned = true;
    if (username.length == 0)
    {
        username = '執行人員';
        Assigned = false;
    }

    // timeline-footer
    new_html = '';
    new_html += '<div class="timeline-footer">';

    new_html += '<div id=timeline_footer_'+event.firedeventid+'>';
    new_html += "<button type='button' class='btn btn-box-tool' onclick='click_edit_event("+event.firedeventid+", \""+empid+"\", \""+username+"\", \""+comment+"\");return false;'><i class='fa fa-edit'></i></button>&nbsp;";

    if (Assigned)
        new_html += '<a class="btn btn-primary btn-xs">'+username+'</a>&nbsp;';
    else
        new_html += '<a class="btn btn-warning btn-xs">'+username+'</a>&nbsp;';

    new_html += '<textarea class="form-control" rows="1" placeholder="輸入註解..."  disabled>'+comment+'</textarea>';
    new_html += '</div>';

    new_html += '</div>';
    return new_html;
}

function click_edit_event(firedeventid, empid, username, comment)
{
    console.log("click_edit_event() firedeventid:"+firedeventid+" empid:"+empid);

    gl_EditMode = true;

    var id = 'timeline_footer_'+firedeventid;
    var timeline_footer = document.getElementById(id);

    while (timeline_footer.firstChild)
    {
        timeline_footer.removeChild(timeline_footer.firstChild);
    }

    // save button
    // '<button type="button" class="btn btn-box-tool" onclick="click_edit_event('+event.firedeventid+');return false;"><i class="fa fa-edit"></i></button>&nbsp;';
    var save_btn = document.createElement("button");
    save_btn.setAttribute("type","button");
    save_btn.setAttribute("class","btn btn-box-tool");
    save_btn.setAttribute("onclick","click_save_event("+ firedeventid +");return false;");
    var imgSave = document.createElement("i");
    imgSave.setAttribute("class","fa fa-save");
    save_btn.appendChild(imgSave);
    timeline_footer.appendChild(save_btn);

    // dropdown list
    // '<a class="btn btn-primary btn-xs">'+userid+'</a>&nbsp;';

    var div = document.createElement("div");
    div.setAttribute("class","btn-group");

    var div_btn = document.createElement("button");
    div_btn.setAttribute("type","button");
    //div_btn.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_btn.setAttribute("class","btn btn-block btn-primary btn-xs dropdown-toggle");
    div_btn.setAttribute("data-toggle","dropdown");
    div_btn.setAttribute("data-id",empid);
    div_btn.setAttribute("id","dropdown_cur_user_"+firedeventid);
    div_btn.textContent = username;
    div.appendChild(div_btn);

    var div_btn_span = document.createElement("span");
    div_btn_span.setAttribute("class","caret");
    div_btn.appendChild(div_btn_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_ul.setAttribute("id","dropdown_user_"+firedeventid);
    div.appendChild(div_ul);
    timeline_footer.appendChild(div);

    //add_employee_dropdown_list(div_ul, firedeventid, empid, username);
    add_user_dropdown_list(div_ul, firedeventid, empid, username);

    // textarea
    //'<textarea class="form-control" rows="1" placeholder="輸入註解..."  disabled>'+comment+'</textarea>';
    var textarea = document.createElement("textarea");
    textarea.setAttribute("class","form-control");
    textarea.setAttribute("rows","1");
    textarea.setAttribute("id","comment_"+firedeventid);
    textarea.textContent = comment;
    timeline_footer.appendChild(textarea);

}

function add_user_dropdown_list(dropdown_users_ul, firedeventid, empid, username)
{
    var item_li = create_dropdown_user_item("-1", "不指定", false, firedeventid);
    dropdown_users_ul.appendChild(item_li);

    var userid = parseInt(empid);

    for (var i = 0; i < gl_user_info.length; i++)
    {
        var defaultchecked = false;
        if ( gl_user_info[i].userid == userid )
            defaultchecked = true;

        var item_li = create_dropdown_user_item(gl_user_info[i].userid, gl_user_info[i].cname, defaultchecked, firedeventid);
        dropdown_users_ul.appendChild(item_li);
    }
}

function create_dropdown_user_item(userid, cname, defaultchecked, firedeventid)
{
    //console.log("create_dropdown_tag_item() prefix:"+prefix+" nodeid:"+nodeid+" macaddress:"+macaddress+" type:"+type+" serno:"+serno);

    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", userid);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_user_click(\""+ firedeventid +"\", \""+ userid +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_user_"+userid);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_user_click(\""+ firedeventid +"\", \""+ userid +"\");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+cname));
    item_li.appendChild(item_click);

    return item_li;
}


var glDropdown_checkbox_user_click = false;
function dropdown_user_click(firedeventid, userid)
{
    console.log("dropdown_user_click() firedeventid:"+firedeventid+" userid:"+userid);

    // uncheck anothers
    var index;
    var showText = "";
    var index;
    for	(index = 0; index < gl_user_info.length; index++)
    {
        if (userid == gl_user_info[index].userid)
        {
            showText = gl_user_info[index].cname + " "+ " ";
            document.getElementById("dropdown_checkbox_user_"+gl_user_info[index].userid).checked = true;
        }
        else
            document.getElementById("dropdown_checkbox_user_"+gl_user_info[index].userid).checked = false;
    }

    var cur_user = document.getElementById("dropdown_cur_user_"+firedeventid);
    if (userid == -1)
    {
        document.getElementById("dropdown_checkbox_user_-1").checked = true;
        cur_user.setAttribute("data-id", "-1");
        cur_user.innerHTML = '<span class="caret"></span>';
    }
    else
    {
        cur_user.setAttribute("data-id", userid);
        cur_user.innerHTML = showText + '<span class="caret"></span>';
        //document.getElementById("dropdown_cur_user_"+firedeventid).innerHTML = showText + '<span class="caret"></span>';
    }

    console.log("dropdown_user_click() firedeventid:"+firedeventid+" -> userid:"+userid);

    glDropdown_checkbox_user_click = false;
}

function dropdown_checkbox_user_click(firedeventid, userid)
{
    glDropdown_checkbox_user_click = true;
}

function click_save_event(firedeventid)
{
    //console.log("click_save_event() firedeventid:"+firedeventid);

    var ui_id = "dropdown_cur_user_"+firedeventid;

    var userid = $("#"+ui_id).attr("data-id");

    var textarea_id = "comment_"+firedeventid;
    var textarea = document.getElementById(textarea_id);

    var comment = textarea.value;

    //console.log("click_save_event() empid:"+empid +" comment:"+comment);

    update_FiredEvents_user(firedeventid, userid, comment);

    gl_EditMode = false;


    reload_events();

}

function reload_events()
{
    gl_allevents = [];
    firedEvents_offset = 0;
    gl_ui_firedEvents_offset = 0;
    showevents_firstload();
}

function update_FiredEvents_user(firedeventid, userid, comment)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['UPDATE_FIREDEVENT_USER'] = 1;
    jsondata['ID'] = firedeventid;
    jsondata['EMPID'] = userid;
    jsondata['COMMENT'] = comment;

    var targetURL = gl_target_server + "/php/eventfire.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      success:
        function(response)
        {
            if ( response.status == 400 )
            {
                alert("update Error! response = " + JSON.stringify(response));
            }
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });

   return false;
}

function findUserName(empid)
{
    var UserName = "";

    var userid = parseInt(empid);

    for (var i = 0; i < gl_user_info.length; i++)
    {
        if ( userid == gl_user_info[i].userid)
        {
            UserName = gl_user_info[i].cname;
            break;
        }
    }

    return UserName;
}

function convert_action_name(action_type)
{
    if (action_type == "Enter")
    {
        return "進入";
    }
    else
    if (action_type == "Leave")
    {
        return "離開";
    }
    else
    if (action_type == "Stay")
    {
        return "停留";
    }
    else
    if (action_type == "Alert")
    {
        return "按下警急按鈕";
    }
    else
    if (action_type == "AlertTagNotMoving")
    {
        return "無動作警告";
    }
    else
    if (action_type == "AlertLayDown")
    {
        return "躺臥警告";
    }
    else
    if (action_type == "Cancel" || action_type == "cancel")
    {
        return "按下取消按鈕";
    }
    else
    if (action_type == "Finish")
    {
        return "完成";
    }
    else
    if (action_type == "Exceed")
    {
        return "超過巡檢時間";
    }
    else
    if (action_type == "shorttime")
    {
        return "巡檢時間過短";
    }
    else
    if (action_type == "longtime")
    {
        return "巡檢時間過長";
    }
    else
    if (action_type == "CallOut")
    {
        return "按下呼換鈕";
    }
    else
    if (action_type == "PowerOff")
    {
        return "拔除電源";
    }
    else
    if (action_type == "PowerOn")
    {
        return "接通電源";
    }
    else
    if (action_type == "OpenDoor")
    {
        return "開門";
    }
    else
    if (action_type == "CloseDoor")
    {
        return "關門";
    }
    else
    if (action_type == "iCoverOpenDoor")
    {
        return "異常移動";
    }
    else
    if (action_type == "iCoverCloseDoor")
    {
        return "定時回報";
    }
    else
    if (action_type == "EnterDoor")
    {
        return "進入開門區域";
    }
    else
    if (action_type == "ExitDoor")
    {
        return "離開開門區域";
    }
    else
        return action_type;
}

function convert_IssueTime_name(IssueTime)
{
    if (IssueTime == "Immediately")
    {
        return "立刻";
    }
    else
    if (IssueTime == "1 minute")
    {
        return "1分鐘";
    }
    else
    if (IssueTime == "5 minutes")
    {
        return "5分鐘";
    }
    else
    if (IssueTime == "10 minutes")
    {
        return "10分鐘";
    }
    else
    {
        return IssueTime;
    }
}

function showevents_firstload()
{
    //document.getElementById("AjaxLoadMode").click();

	curURl = window.location.href;
	if (curURl.indexOf("vilseventtimeline") >= 0)
	{
	    document.getElementById("AjaxLoadMode").click();
	}
}

function fallbackCopyTextToClipboard(cardid, datetime)
{
    var textArea = document.createElement("textarea");
    textArea.value = cardid;

    // Avoid scrolling to bottom
    textArea.style.top = "0";
    textArea.style.left = "0";
    textArea.style.position = "fixed";

    document.body.appendChild(textArea);
    textArea.focus();
    textArea.select();

    var id = "cardid_copy_alert_"+cardid+"_"+datetime;
    var alert_msg_ui = document.getElementById(id);

    try {
        var successful = document.execCommand('copy');

        if ( successful )
            alert_msg_ui.textContent = "已複製";
        else
            alert_msg_ui.textContent = "無法複製";

        setTimeout(clear_alert_msg, 3000, cardid, datetime);

        var msg = successful ? 'successful' : 'unsuccessful';
        console.log('Fallback: Copying text command was ' + msg);
    }
    catch (err)
    {
        console.error('Fallback: Oops, unable to copy', err);
        alert_msg_ui.textContent = "無法複製";
        setTimeout(clear_alert_msg, 3000, cardid, datetime);
    }

    document.body.removeChild(textArea);
}

function copyTextToClipboard(cardid, datetime)
{
    if (!navigator.clipboard)
    {
        fallbackCopyTextToClipboard(cardid, datetime);
        return;
    }

    var id = "cardid_copy_alert_"+cardid+"_"+datetime;

    navigator.clipboard.writeText(cardid).then(function()
    {
        console.log('Async: Copying to clipboard was successful!');
        var alert_msg_ui = document.getElementById(id);
        alert_msg_ui.textContent = "已複製";

        setTimeout(clear_alert_msg, 3000, cardid, datetime);
    },
    function(err)
    {
        console.error('Async: Could not copy text: ', err);
        alert_msg_ui.textContent = "無法複製";

        setTimeout(clear_alert_msg, 3000, cardid, datetime);
    });
}

function clear_alert_msg(cardid, datetime)
{
    var ui_id = "cardid_copy_alert_"+cardid+"_"+datetime;
    document.getElementById(ui_id).textContent = '';
}
//
// auto click at first load page.
//
showevents_firstload();


function open_eventtimeline_page()
{
    var win = window.open('/index.php/vilseventtimeline/'+project_id, '_blank');
    win.focus();
}
