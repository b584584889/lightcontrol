/*
 * Author: ikki Chung
 * Date: 21 Sep 2016
 * Description:
 *      SMIMS VILS RTLS Solutions event time line
 **/

var firedEvents_count = 10;
var firedEvents_offset = 0;
var glUpdatetime_countdown = 1;

//
// first load page.
//
period_check_new_fired_events();

function period_check_new_fired_events()
{
    check_new_fired_events();
    setTimeout(period_check_new_fired_events, 30000);// 30 seconds
}


function check_new_fired_events_countdown(count)
{
    if (glUpdatetime_countdown <= 0)
    {

        //console.log("check_new_fired_events_countdown() count:"+count);

        glUpdatetime_countdown = count;
        check_new_fired_events();
    }
}
//
// check new fired event periodically
//
function check_new_fired_events()
{
    //
    // check new item
    //
    //
    // API: /php/eventfire.php?unread=1&prettystring=1&count=10&offset=0&markasread=0
    //


    var targetURL = gl_target_server + '/php/eventfire.php?project_id='+project_id+'&unread=1&prettystring=1&count='+firedEvents_count+'&offset='+firedEvents_offset;
    $.getJSON(targetURL,
    function(data)
    {
        var new_count = 0;
        var new_html = '<ul class="menu">';

        $.each(data, function(itemindex, event)
        {
            //var eventstr1 = event.eventjson.replace(/'/g, '"');
            //var eventstr2 = eventstr1.replace(/'/g, '"');
            //var eventjson = JSON.parse(eventstr2);
            var eventjson = event.eventjson;

            //var prettystr1 = event.prettyjson.replace(/'/g, '"');
            //var prettystr2 = prettystr1.replace(/'/g, '"');
            //var prettyjson = JSON.parse(prettystr2);
            var prettyjson = event.prettyjson;

            //new_html += '<li><a href="/index.php/eventtimeline/'+parseInt(event.projectid)+'"></i>';
            new_html += '<li><a href="javascript:void(0)" onclick="show_eventtimeline('+parseInt(event.projectid)+');return false;"></i>';

            // if (eventjson.action == 'Enter')
            //     new_html += '<i class="fa fa-sign-in text-aqua"></i>';
            // else
            // if (eventjson.action == 'Leave')
            //     new_html += '<i class="fa fa-share-square-o text-red"></i>';
            // else
            // if (eventjson.action == 'Stay')
            //     new_html += '<i class="fa fa-street-view text-yellow"></i>';
            // else
            // if (eventjson.action == 'unknowncard')
            //     new_html += '<i class="fa fa-warning"></i>';
            // else
            //     new_html += '<i class="fa fa-street-view text-yellow"></i>';
            if (eventjson.action == 'Enter')
                new_html += '<i class="fa fa-sign-in text-aqua"></i>';
            else
            if (eventjson.action == 'Leave')
                new_html += '<i class="fa fa-share-square-o text-yellow-active"></i>';
            else
            if (eventjson.action == 'Stay')
                new_html += '<i class="fa fa-street-view text-aqua-active"></i>';
            else
            if (eventjson.action == 'Alert' || eventjson.action == 'AlertTagNotMoving' || eventjson.action == 'AlertLayDown')
                new_html += '<i class="fa fa-bell-o text-red"></i>';
            else
            if (eventjson.action == 'Cancel')
                new_html += '<i class="fa fa-bell-slash-o text-blue"></i>';
            else
            if (eventjson.action == 'CallOut')
                new_html += '<i class="fa fa-hand-paper-o text-green"></i>';
            else
            if (eventjson.action == 'PowerOff')
                new_html += '<i class="fa fa-flash text-red"></i>';
            else
            if (eventjson.action == 'PowerOn')
                new_html += '<i class="fa fa-flash text-green"></i>';
            else
            if (eventjson.action == 'OpenDoor')
                new_html += '<i class="fa fa-unlock text-red"></i>';
            else
            if (eventjson.action == 'CloseDoor')
                new_html += '<i class="fa fa-lock text-green"></i>';
            else
            if (eventjson.action == 'iCoverOpenDoor')
                new_html += '<i class="fa fa-unlock text-red"></i>';
            else
            if (eventjson.action == 'iCoverCloseDoor')
                new_html += '<i class="fa fa-lock text-green"></i>';
            else
            if (eventjson.action == 'EnterDoor')
                new_html += '<i class="fa fa-sign-in text-red"></i>';
            else
            if (eventjson.action == 'ExitDoor')
                new_html += '<i class="fa fa-sign-out text-green"></i>';
            else
            if (eventjson.action == 'SensorAlert')
                new_html += '<i class="fa fa-rss text-red"></i>';
            else
            if (eventjson.action == 'unknowncard')
                new_html += '<i class="fa fa-warning text-red"></i>';
            else
            if (eventjson.action == 'BodyInfoAlertNormal')
                new_html += '<i class="fa fa-street-view text-green"></i>';
            else
            if (eventjson.action == 'BodyInfoAlertWarning')
                new_html += '<i class="fa fa-street-view text-yellow"></i>';
            else
            if (eventjson.action == 'BodyInfoAlertDanger')
                new_html += '<i class="fa fa-street-view text-red"></i>';
            else
                new_html += '<i class="fa fa-street-view text-yellow"></i>';

            if (prettyjson.type == 'inspectionpoint')
            {
                new_html += '&nbsp;&nbsp;'+prettyjson.tagname;
                new_html += '&nbsp;&nbsp;'+convert_action_name(prettyjson.actionname);
                new_html += '&nbsp;&nbsp;巡檢點:'+ prettyjson.inspectionpointname+'&nbsp;';
            }
            else
            if (prettyjson.type == 'inspectionpointtravel')
            {
                if (prettyjson.tagname.length > 0)
                    new_html += '&nbsp;&nbsp;'+prettyjson.tagname;
                new_html += '&nbsp;&nbsp;'+convert_action_name(prettyjson.actionname)+'&nbsp;';
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
                new_html += '&nbsp;&nbsp;'+prettyjson.groupname;
                new_html += '&nbsp;&nbsp;'+ convert_action_name(prettyjson.actionname)+'&nbsp;';
            }
            else
            if (prettyjson.type == 'rfidcard')
            {
                //new_html += '&nbsp;&nbsp;'+prettyjson.cardid;
                new_html += '&nbsp;&nbsp;'+ convert_action_name(prettyjson.actionname)+'&nbsp;';
            }
            else
            if (prettyjson.type == 'nfccard')
            {
                //new_html += '&nbsp;&nbsp;'+prettyjson.cardid;
                new_html += '&nbsp;&nbsp;'+ convert_action_name(prettyjson.actionname)+'&nbsp;';
            }
            else
            {

                new_html += '&nbsp;&nbsp;'+prettyjson.nodename;

                if( prettyjson.locatorname.length > 0 && prettyjson.locatorname != prettyjson.nodename)
                    new_html += '&nbsp;&nbsp;在'+prettyjson.locatorname;

                //new_html += '&nbsp;&nbsp;'+prettyjson.actionname;
                new_html += '&nbsp;&nbsp;'+convert_action_name(prettyjson.actionname);
                if( prettyjson.areaname.length > 0 )
                    new_html += '&nbsp;&nbsp;'+prettyjson.areaname;
            }



            new_html += '</a></li>';

            new_count++;
        });

        new_html += '</ul>';

        $('.firedevent_bubble_content-0').html(new_html);

        //alert('new_count='+new_count);

        if (new_count == 0)
            $("#main_top_event_num_bubble").hide();
        else
            $("#main_top_event_num_bubble").show();

        $("#main_top_event_num_bubble").html(new_count);
        $("#main_top_event_num").html('You have '+new_count+' notifications');

    })
    .error(function() {
        setTimeout(check_new_fired_events, 3000);
    })
    .complete(function() {
        glUpdatetime_countdown--;
        if (glUpdatetime_countdown > 0)
            setTimeout(check_new_fired_events, 1000);
    });

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
    if (action_type == "unknowncard")
    {
        return "發現未註冊卡片";
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
