/*
 * Author: ikki Chung
 * Date: 2020 04 22
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var LOAD_PAGE_NUM = 20;
var LOAD_DEVICE_NUM = 40;

var glTags = [];
var glLocators = [];

var gl_tag_num = 0;
var gl_locator_num = 0;

var gl_tag_load_index = 0;
var gl_locator_load_index = 0;

var glSurvey_List = [];
var glSurveyEvent_List = [];
var glPushNotifyMessageRecord_List = [];
var glSurveyResult_List = [];
// var glPushNotifyImage_List = [];
var glStorageImage_List = [];
var glMessageStatus_List = [];

var glCurrentPage = 1;
var glReloadTimes = 0;

// start loading
check_loading_status();

function check_loading_status()
{
    //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" CurrentLoadIndex:" + CurrentLoadIndex);
    switch(CurrentLoadIndex)
    {
        case 0:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                load_survey();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                load_surveyevent();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                show_surveyevents();

                CurrentLoadIndex = CurrentLoadIndex + 1; // next state
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                load_devices();
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                load_surveyresults();
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                show_surveyresults();
                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 6:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                load_PushNotify_uploadimage();
                load_user_uploadimage();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 7:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_message_status();
            }
        break;
        case 8:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_message_status();

                CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(check_loading_status, 100);
    else
    {
        console.log("check_loading_status() Stop.");
    }
}

function load_survey()
{
    var targetURL = gl_target_server + "/php/survey.php";

    glSurvey_List = [];

    $.getJSON(targetURL, 'loadsurvey=1'+'&project_id='+project_id+'&surveyid='+show_surveyid+'&accountid='+accountid, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            glSurvey_List.push(record);
        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .complete(function() {
    });
}

function load_surveyevent()
{
    var targetURL = gl_target_server + "/php/survey.php";

    glSurveyEvent_List = [];

    $.getJSON(targetURL, 'loadsurveyevent=1'+'&project_id='+project_id+'&surveyid='+show_surveyid+'&hashkey='+show_hashkey+'&accountid='+accountid, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            glSurveyEvent_List.push(record);
        });
    })
    .success(function() {
        load_PushNotifyMessageRecord();
    })
    .error(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .complete(function() {
    });
}

function load_PushNotifyMessageRecord()
{
    var targetURL = gl_target_server + "/php/pushnotify.php";

    glPushNotifyMessageRecord_List = [];

    var dbids = "";
    for(var index=0;index<glSurveyEvent_List.length;index++)
    {
        var SurveyEvent = glSurveyEvent_List[index];

        if (index != 0)
            dbids += ",";

        dbids += SurveyEvent.PushNotifyMessageRecorddbId;
    }

    $.getJSON(targetURL, 'loadmessagerecord=1'+'&project_id='+project_id+'&dbids='+dbids, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            glPushNotifyMessageRecord_List.push(record);
        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .complete(function() {
    });
}

function load_surveyresults()
{
    var targetURL = gl_target_server + "/php/survey.php";

    glSurveyResult_List = [];

    var hashkeys = "";
    for(var index=0;index<glSurveyEvent_List.length;index++)
    {
        var SurveyEvent = glSurveyEvent_List[index];

        if (index != 0)
            hashkeys += ",";

        hashkeys += SurveyEvent.hashkey;
    }

    $.getJSON(targetURL, 'loadsurveyresults=1'+'&project_id='+project_id+"&hashkeys="+hashkeys+"&loaduser=1"+'&accountid='+accountid, function(data)
    {
        $.each(data, function(hashkey, record)
        {
            var result = {};
            result.hashkey = hashkey;
            result.record = record;
            glSurveyResult_List.push(result);
        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .complete(function() {
    });
}

function show_surveyevents()
{
    var table_body_ui = document.getElementById('SURVEY_EVENT_TABLE_BODY');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    console.log("show_surveyevent() glSurveyEvent_List.length:"+glSurveyEvent_List.length);

    var show_count = 0;
    for(var index=0;index<glSurveyEvent_List.length;index++)
    {
        show_count += show_surveyevent(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + show_count+1, glSurveyEvent_List[index]);
        show_surveyeventitem(glSurveyEvent_List[index]);
    }
}

function show_surveyevent(table_body_ui, ui_index, record)
{
    var cellIndex = 0;

    console.log("show_surveyevent() surveyid:"+record.surveyid);

    var survey = find_survey(record.surveyid);
    if (survey == null)
    {
        return 0;
    }

    console.log("show_surveyevent() survey.type:"+survey.type + " survey.title:"+survey.title);

    var row = document.createElement('tr');
    row.setAttribute("id", record.dbId+"_TABLE_ROW");
    table_body_ui.appendChild(row);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // 紀錄id
    row.appendChild(document.createElement('td'));
    var titleDiv = document.createElement('div');
    titleDiv.appendChild(document.createTextNode(record.eventid));
    row.cells[cellIndex].appendChild(titleDiv);
    cellIndex++;

    // 事項名稱
    row.appendChild(document.createElement('td'));
    var titleDiv = document.createElement('div');
    titleDiv.appendChild(document.createTextNode(survey.title));
    row.cells[cellIndex].appendChild(titleDiv);
    cellIndex++;

    // 事項說明
    row.appendChild(document.createElement('td'));
    var descriptionDiv = document.createElement('div');
    descriptionDiv.setAttribute("id", "description_"+survey.surveyid);
    descriptionDiv.appendChild(document.createTextNode(survey.description));
    row.cells[cellIndex].appendChild(descriptionDiv);
    cellIndex++;

    // 選擇類型
    var type1 = false;
    var type2 = false;
    row.appendChild(document.createElement('td'));
    var typeDiv = document.createElement('div');
    typeDiv.setAttribute("id", "type_"+survey.surveyid);
    row.cells[cellIndex].appendChild(typeDiv);

    if ( survey.type === '0' )
    {
        type1 = true;
        typeDiv.appendChild(document.createTextNode("單選"));
    }
    if ( survey.type === '1' )
    {
        type2 = true;
        typeDiv.appendChild(document.createTextNode("複選"));
    }
    cellIndex++;

    // 事項列表
    // row.appendChild(document.createElement('td'));
    // var survey_item = show_survey_item(survey.surveyid, survey.items, record.hashkey);
    // row.cells[cellIndex].appendChild(survey_item);
    // cellIndex++;

    // deadline
    row.appendChild(document.createElement('td'));
    if (record.deadline != undefined)
    {
        if (record.deadline.length > 0)
        {
            var deadline_date = document.createElement("span");
            deadline_date.setAttribute("class","label label-info");
            deadline_date.textContent = convertPrettyDateOnly(record.deadline, "+00:00");
            row.cells[cellIndex].appendChild(deadline_date);
            var deadline_time = document.createElement("span");
            deadline_time.setAttribute("class","label label-warning");
            deadline_time.textContent = convertPrettyTimeOnly(record.deadline, "+00:00");
            row.cells[cellIndex].appendChild(deadline_time);
            // var badge_date = document.createElement("span");
            // badge_date.setAttribute("class","label label-info");
            // badge_date.textContent = record.deadline;
            // row.cells[cellIndex].appendChild(badge_date);
        }
    }
    cellIndex++;

    // 推播時間
    row.appendChild(document.createElement('td'));
    var badge_date = document.createElement("span");
    badge_date.setAttribute("class","label label-info");
    badge_date.textContent = convertPrettyDateOnly(record.DateTime, "+00:00");
    row.cells[cellIndex].appendChild(badge_date);
    var badge_time = document.createElement("span");
    badge_time.setAttribute("class","label label-warning");
    badge_time.textContent = convertPrettyTimeOnly(record.DateTime, "+00:00");
    row.cells[cellIndex].appendChild(badge_time);
    // var badge_date = document.createElement("span");
    // badge_date.setAttribute("class","label label-success");
    // badge_date.textContent = record.DateTime;
    // row.cells[cellIndex].appendChild(badge_date);
    cellIndex++;

    // 推播紀錄id
    var td_img = document.createElement('td');
    td_img.setAttribute("id", "show_img_area");
    row.appendChild(td_img);
    //row.cells[cellIndex].appendChild(document.createTextNode(""+record.PushNotifyMessageRecorddbId));
    cellIndex++;

    // 操作
    // row.appendChild(document.createElement('td'));
    //
    // var checkBtn = document.createElement("button");
    // checkBtn.setAttribute("title","查看詳細資訊");
    // checkBtn.setAttribute("href","javascript:;");
    // checkBtn.setAttribute("onclick","show_surveyresult("+ project_id +",\""+record.hashkey+"\");return false;");
    // var imgCheck = document.createElement("i");
    // imgCheck.setAttribute("class","fa fa-calendar-check-o ");
    // checkBtn.appendChild(imgCheck);
    // row.cells[cellIndex].appendChild(checkBtn);
    // cellIndex++;

    return 1;
}

function load_devices()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_locator_num = parseInt(data['LOC_LOCATOR_NUM']);

        gl_tag_load_index = 0;
        gl_locator_load_index = 0;
    })
    .success(function() {
        setTimeout(load_tags, 10);
    })
    .error(function() {
        setTimeout(load_devices, 1000);
    })
    .complete(function() {
    });
}

function load_tags()
{
    if (gl_tag_load_index >= gl_tag_num)
    {
        gl_tag_load_index = 0;
        setTimeout(load_locators, 10);
        return;
    }
    if (gl_tag_load_index == 0)
    {
        glTags = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadtag=1&s='+gl_tag_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var tag_num = parseInt(data['LOC_TAG_NUM']);

        if (gl_tag_load_index == 0 && tag_num == 0)
        {
            gl_tag_num = 0;
            gl_tag_load_index = 0;
        }

        for (var i = gl_tag_load_index; i < gl_tag_load_index+tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
            var node = data[key];
            if (node == undefined)
                continue;
            var nodeData = node.split(",");

            glTags.push(nodeData);
        }
        gl_tag_load_index += tag_num;

    })
    .success(function() {
        if (gl_tag_load_index >= gl_tag_num)
        {
            gl_tag_load_index = 0;
            setTimeout(load_locators, 10);
        }
        else
        {
            setTimeout(load_tags, 10);
        }
    })
    .error(function() {
        gl_tag_load_index = 0;
        setTimeout(load_tags, 1000);
    })
    .complete(function() {
    });
}

function load_locators()
{
    console.log("load_locators(): gl_locator_num:"+gl_locator_num+" gl_locator_load_index:"+gl_locator_load_index);

    if (gl_locator_load_index >= gl_locator_num)
    {
        gl_locator_load_index = 0;

        CurrentLoadIndex = CurrentLoadIndex + 1;
        return;
    }
    if (gl_locator_load_index == 0)
    {
        glLocators = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadlocator=1&s='+gl_locator_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var locator_num = parseInt(data['LOC_LOCATOR_NUM']);

        //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);
        //var anchor_size = glAnchors.length;
        for (var i = gl_locator_load_index; i < gl_locator_load_index+locator_num; ++i)
        {
            var key = 'LOC_LOCATOR_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            // split data
            var node = data[key];
            var nodeData = node.split(",");

            glLocators.push(nodeData);
        }

        gl_locator_load_index += locator_num;

    })
    .success(function() {
        if (gl_locator_load_index >= gl_locator_num)
        {
            gl_locator_load_index = 0;
            CurrentLoadIndex = CurrentLoadIndex + 1;
        }
        else
        {
            load_locators();
        }
    })
    .error(function() {
        gl_locator_load_index = 0;
        CurrentLoadIndex = CurrentLoadIndex + 1;
        setTimeout(load_locators, 1000);
    })
    .complete(function() {
    });
}

function show_surveyeventitem(record)
{
    var table_body_ui = document.getElementById('SURVEY_ITEM_TABLE_BODY');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    var cellIndex = 0;

    // console.log("show_surveyeventitem() surveyid:"+record.surveyid);

    var survey = find_survey(record.surveyid);
    if (survey == null)
    {
        return 0;
    }

    // console.log("show_surveyeventitem() survey.type:"+survey.type + " survey.title:"+survey.title);

    for(var index=0;index<survey.items.length;index++)
    {
        var item = survey.items[index];
        var survey_tr = document.createElement('tr');
        table_body_ui.appendChild(survey_tr);

        var survey_td_0 = document.createElement('td');
        survey_td_0.appendChild(document.createTextNode(index+1));
        survey_tr.appendChild(survey_td_0);

        var survey_td_1 = document.createElement('td');
        survey_td_1.appendChild(document.createTextNode(item.description));
        survey_tr.appendChild(survey_td_1);

        var survey_td_2 = document.createElement('td');
        survey_tr.appendChild(survey_td_2);

        var survey_td_2_span = document.createElement("span");
        survey_td_2_span.setAttribute("class","label label-danger");
        survey_td_2_span.setAttribute("id","survey_item_"+record.hashkey+"_"+index);
        survey_td_2_span.textContent = '0';
        survey_td_2.appendChild(survey_td_2_span);

        var survey_td_3 = document.createElement('td');
        survey_td_3.setAttribute("id","survey_item_user_"+record.hashkey+"_"+index);
        survey_tr.appendChild(survey_td_3);

        // var survey_td_3_div = document.createElement("div");
        // survey_td_3_div.setAttribute("id","survey_item_user_"+record.hashkey+"_"+index);
        // survey_td_3.appendChild(survey_td_3_div);

    }

    // comment
    var survey_tr = document.createElement('tr');
    table_body_ui.appendChild(survey_tr);

    var survey_td_0 = document.createElement('td');
    survey_td_0.appendChild(document.createTextNode(survey.items.length+1));
    survey_tr.appendChild(survey_td_0);

    var survey_td_1 = document.createElement('td');
    survey_td_1.appendChild(document.createTextNode("說明"));
    survey_tr.appendChild(survey_td_1);

    var survey_td_comment = document.createElement('td');
    survey_td_comment.setAttribute("id","survey_item_comment_"+record.hashkey);
    survey_tr.appendChild(survey_td_comment);

    var survey_td_comment_user = document.createElement('td');
    survey_td_comment_user.setAttribute("id","survey_item_comment_user_"+record.hashkey);
    survey_tr.appendChild(survey_td_comment_user);

}

function show_surveyresults()
{
    //console.log("show_surveyresults() glSurveyResult_List.length:"+glSurveyResult_List.length);

    for(var index=0;index<glSurveyResult_List.length;index++)
    {
        var surveyResult = glSurveyResult_List[index];
        //console.log("show_surveyresults() hashkey:"+surveyResult.hashkey +" surveyResult.record.length:"+surveyResult.record.length);

        //==========================================================================
        // clear old data
        //
        var survey_td_comment_user = document.getElementById("survey_item_comment_user_"+surveyResult.hashkey);
        if (survey_td_comment_user != undefined)
        {
            while (survey_td_comment_user.firstChild)
            {
                survey_td_comment_user.removeChild(survey_td_comment_user.firstChild);
            }
        }

        var survey_td_comment = document.getElementById("survey_item_comment_"+surveyResult.hashkey);
        if (survey_td_comment != undefined)
        {
            while (survey_td_comment.firstChild)
            {
                survey_td_comment.removeChild(survey_td_comment.firstChild);
            }
        }

        for(var rindex=0;rindex<surveyResult.record.length;rindex++)
        {
            var record = surveyResult.record[rindex];
            var result = record.results.split(",");

            for(var i=0;i<result.length;i++)
            {
                var survey_td_3 = document.getElementById("survey_item_user_"+surveyResult.hashkey+"_"+i);
                if (survey_td_3 != undefined)
                {
                    while (survey_td_3.firstChild)
                    {
                        survey_td_3.removeChild(survey_td_3.firstChild);
                    }
                }
            }
        }
        //==========================================================================

        var UserResults = [];
        var AllResults = [];
        var AllComments = [];

        for(var rindex=0;rindex<surveyResult.record.length;rindex++)
        {
            var record = surveyResult.record[rindex];
            var result = record.results.split(",");

            var survey_td_comment_user = document.getElementById("survey_item_comment_user_"+surveyResult.hashkey);
            if (survey_td_comment_user != undefined)
            {
                if ( record.comment.length > 0 )
                {
                    AllComments.push(record.comment);

                    var survey_td_comment_div = document.createElement("div");
                    survey_td_comment_user.appendChild(survey_td_comment_div);

                    var survey_item_comment_span = document.createElement("span");
                    survey_item_comment_span.setAttribute("class","label label-primary");
                    survey_item_comment_span.textContent = record.comment ;
                    survey_td_comment_div.appendChild(survey_item_comment_span);

                    var survey_item_user_span = document.createElement("span");
                    survey_item_user_span.setAttribute("class","label label-success");
                    //survey_item_user_span.textContent = getNodeNameByTopic(record.targetid);
                    survey_item_user_span.textContent = record.name;
                    survey_td_comment_div.appendChild(survey_item_user_span);

                    var badge_date = document.createElement("span");
                    badge_date.setAttribute("class","label label-info");
                    badge_date.textContent = convertPrettyDateOnly(record.datetime, "+00:00");
                    survey_td_comment_div.appendChild(badge_date);

                    var badge_time = document.createElement("span");
                    badge_time.setAttribute("class","label label-warning");
                    badge_time.textContent = convertPrettyTimeOnly(record.datetime, "+00:00");
                    survey_td_comment_div.appendChild(badge_time);
                }
            }

            //console.log("show_surveyresults() messageid:"+record.messageid +" comment:"+record.comment +" results:"+record.results);

            if (UserResults[record.topic] == undefined)
            {
                UserResults[record.topic] = 1;
            }
            else
            {
                continue;
            }

            for(var i=0;i<result.length;i++)
            {
                var data = parseInt(result[i]);
                if (AllResults[i] == undefined)
                    AllResults[i] = 0;
                AllResults[i] += data;

                // console.log("show_surveyresults() i:"+i +" data:"+data +" hashkey:"+surveyResult.hashkey+" record.targetid:"+record.targetid);
                var survey_td_3 = document.getElementById("survey_item_user_"+surveyResult.hashkey+"_"+i);
                if (survey_td_3 != undefined)
                {
                    if ( data == 1 )
                    {
                        var survey_td_3_div = document.createElement("div");
                        survey_td_3.appendChild(survey_td_3_div);

                        var survey_item_user_span = document.createElement("span");
                        survey_item_user_span.setAttribute("class","label label-success");
                        //survey_item_user_span.textContent = getNodeNameByTopic(record.targetid);
                        survey_item_user_span.textContent = record.name;
                        survey_td_3_div.appendChild(survey_item_user_span);

                        var badge_date = document.createElement("span");
                        badge_date.setAttribute("class","label label-info");
                        badge_date.textContent = convertPrettyDateOnly(record.datetime, "+00:00");
                        survey_td_3_div.appendChild(badge_date);

                        var badge_time = document.createElement("span");
                        badge_time.setAttribute("class","label label-warning");
                        badge_time.textContent = convertPrettyTimeOnly(record.datetime, "+00:00");
                        survey_td_3_div.appendChild(badge_time);

                    }
                }
            }
        }
        //console.log("show_surveyresults() AllResults.length:"+AllResults.length);

        for(var rindex=0;rindex<AllResults.length;rindex++)
        {
            //console.log("show_surveyresults() rindex:"+rindex+" AllResults="+AllResults[rindex]);
            //console.log("show_surveyresults() id:survey_item_"+surveyResult.hashkey+"_"+rindex);

            var survey_td_2_span = document.getElementById("survey_item_"+surveyResult.hashkey+"_"+rindex);
            if (survey_td_2_span != undefined)
            {
                survey_td_2_span.textContent = AllResults[rindex];
                if (AllResults[rindex] == 0)
                {
                    survey_td_2_span.setAttribute("class","label label-danger");
                }
                else
                {
                    survey_td_2_span.setAttribute("class","label label-info");
                }
            }
        }

        for(var rindex=0;rindex<AllComments.length;rindex++)
        {
            if (survey_td_comment != undefined)
            {
                var survey_td_comment_span = document.createElement("span");
                survey_td_comment_span.setAttribute("class","label label-primary");
                survey_td_comment.appendChild(survey_td_comment_span);
                survey_td_comment_span.textContent = AllComments[rindex];
            }
        }
    }

    if (glReloadTimes < 2)
    {
        setTimeout(reload_surveyresults, 10000);
        glReloadTimes++;
    }
}

function load_PushNotify_uploadimage()
{
    // glPushNotifyImage_List = [];

    // var messagehashkey = '';
    // for(var index=0;index<glSurveyResult_List.length;index++)
    // {
    //     var surveyResult = glSurveyResult_List[index];
    //
    //     for(var rindex=0;rindex<surveyResult.record.length;rindex++)
    //     {
    //         var record = surveyResult.record[rindex];
    //
    //         messagehashkey = record.messagehashkey;
    //         break;
    //     }
    //     break;
    // }

    var messagehashkey = '';
    for(var index=0;index<glPushNotifyMessageRecord_List.length;index++)
    {
        var record = glPushNotifyMessageRecord_List[index];
        messagehashkey = record.messagehashkey;
        break;
    }

    // console.log("load_PushNotify_uploadimage() messagehashkey:"+messagehashkey);

    if (messagehashkey.length == 0)
        return;

    var targetURL = gl_target_server + "/php/pushnotify.php";
    var p = $.getJSON(targetURL, 'loadnotifyimage=1&project_id='+project_id+'&messagehashkey='+messagehashkey, function(data)
    {
        // console.log("load_PushNotify_uploadimage() data.imageuid:"+data.imageuid+" imageuid.length:"+data.imageuid.length);

        if (data.imageuid.length > 0 )
        {
            load_PushNotify_images(data.imageuid);
        }
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_PushNotify_images(imageuid)
{
    console.log("load_PushNotify_images() imageuid:"+imageuid);

    var imageuidlist = imageuid.split(",");

    for(var i=0;i<imageuidlist.length;i++)
    {
        var uid = imageuidlist[i];

        load_PushNotify_image(uid);
    }
}

function load_PushNotify_image(imageuid)
{
    console.log("load_PushNotify_image() imageuid:"+imageuid);

    var targetURL = gl_target_server + "/php/storage.php";
    var p = $.getJSON(targetURL, 'loadsingleimage=1&project_id='+project_id+'&imageuid='+imageuid, function(storageImage)
    {

        var ui_id = 'show_img_area_'+imageuid;
        var show_img_area_image_ui = document.getElementById(ui_id);
        if ( show_img_area_image_ui == undefined )
        {

            // console.log("load_PushNotify_image() show_img_area_image_ui("+ show_img_area_image_ui +")  == undefined");

            var show_img_area = document.getElementById("show_img_area");

            var td_filetype = create_data_image(storageImage.id, storageImage.description, storageImage.thumbpath, storageImage.imagetype, storageImage.imagesize);
            td_filetype.setAttribute("id", ui_id);
            show_img_area.appendChild(td_filetype);
        }
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_user_uploadimage()
{
    glStorageImage_List = [];

    // var messagehashkey = '';
    // for(var index=0;index<glSurveyResult_List.length;index++)
    // {
    //     var surveyResult = glSurveyResult_List[index];
    //
    //     for(var rindex=0;rindex<surveyResult.record.length;rindex++)
    //     {
    //         var record = surveyResult.record[rindex];
    //
    //         messagehashkey = record.messagehashkey;
    //         break;
    //     }
    //     break;
    // }

    var messagehashkey = '';
    for(var index=0;index<glPushNotifyMessageRecord_List.length;index++)
    {
        var record = glPushNotifyMessageRecord_List[index];
        messagehashkey = record.messagehashkey;
        break;
    }

    // console.log("load_user_uploadimage() messagehashkey:"+messagehashkey);

    if (messagehashkey.length == 0)
        return;

    var targetURL = gl_target_server + "/php/storage.php";
    var p = $.getJSON(targetURL, 'loadnotifyimage=1&project_id='+project_id+'&messagehashkey='+messagehashkey, function(data)
    {
        $.each(data, function(index, storageimage)
        {
            glStorageImage_List.push(storageimage);
        });
    })
    .success(function() {

        show_user_uploadimage();

    })
    .error(function() {
    })
    .complete(function() {
    });
}

function show_user_uploadimage()
{
    var upload_img_table_body_ui = document.getElementById('UPLOAD_IMAGE_TABLE_BODY');

    for(var index=0;index<glStorageImage_List.length;index++)
    {
        var storageImage = glStorageImage_List[index];

        console.log("show_user_uploadimage() description:"+storageImage.description);

        var thumbimagelink = gl_target_server + "/php/downloadfile.php?imageuid="+ storageImage.imageuid +"&loadthumb=1"

        console.log("show_user_uploadimage() thumbimagelink:"+thumbimagelink);

        var row_id = "upload_img_" + storageImage.topic;

        var row_ui = document.getElementById(row_id);
        if (row_ui == undefined)
        {
            // row_ui.setAttribute("src", thumbimagelink);
            var row_ui = create_upload_img_row_item(index, storageImage);
            upload_img_table_body_ui.appendChild(row_ui);
        }

        update_upload_img_item(storageImage);
    }
}

function create_upload_img_row_item(uiindex, storageImage)
{
    var row_ui = document.createElement('tr');
    row_ui.setAttribute("id","upload_img_" + storageImage.topic);

    var item_td_0 = document.createElement('td');
    item_td_0.appendChild(document.createTextNode(uiindex+1));
    row_ui.appendChild(item_td_0);

    var item_td_1 = document.createElement('td');
    item_td_1.setAttribute("id", "upload_img_td_" + storageImage.topic);
    row_ui.appendChild(item_td_1);

    var item_td_2 = document.createElement('td')
    row_ui.appendChild(item_td_2);

    var item_user_span = document.createElement("span");
    item_user_span.setAttribute("class","label label-success");
    item_user_span.textContent = getUserNameByTopic(storageImage.topic);
    item_td_2.appendChild(item_user_span);

    var badge_date = document.createElement("span");
    badge_date.setAttribute("class","label label-info");
    badge_date.textContent = convertPrettyDateOnly(storageImage.updatetime, "+00:00");
    item_td_2.appendChild(badge_date);

    var badge_time = document.createElement("span");
    badge_time.setAttribute("class","label label-warning");
    badge_time.textContent = convertPrettyTimeOnly(storageImage.updatetime, "+00:00");
    item_td_2.appendChild(badge_time);

    return row_ui;
}

function update_upload_img_item(storageImage)
{
    var image_id = "show_img_" + storageImage.imageuid;

    var image_ui = document.getElementById(image_id);
    if (image_ui != undefined)
    {
        return;
    }

    var item_td_1 = document.getElementById("upload_img_td_" + storageImage.topic);

    var td_filetype = create_data_image(storageImage.id, storageImage.description, storageImage.thumbpath, storageImage.imagetype, storageImage.imagesize);
    td_filetype.setAttribute("id",image_id);
    item_td_1.appendChild(td_filetype);
}

function create_data_image(id, filename, thumbpath, filetype, filesize)
{
    var d = document.createElement("td");

    var data_ul = document.createElement("ul");
    data_ul.setAttribute("class","mailbox-attachments clearfix");
    d.appendChild(data_ul);

    var data_li = document.createElement("li");
    data_ul.appendChild(data_li);

    var span_icon = document.createElement("span");
    span_icon.setAttribute("class","mailbox-attachment-icon has-img");
    data_li.appendChild(span_icon);

    var span_icon_img = document.createElement("img");
    span_icon_img.setAttribute("src","/php/downloadfile.php?loadthumb=1&project_id="+project_id+"&imageid="+id);
    span_icon_img.setAttribute("alt","Attachment");
    span_icon.appendChild(span_icon_img);

    var div_info = document.createElement("div");
    div_info.setAttribute("class","mailbox-attachment-info");
    data_li.appendChild(div_info);

    var div_info_href = document.createElement("a");
    div_info_href.setAttribute("href","/php/downloadfile.php?project_id="+project_id+"&imageid="+id);
    div_info_href.setAttribute("class","mailbox-attachment-name");
    div_info.appendChild(div_info_href);

    var div_info_href_img = document.createElement("i");
    div_info_href_img.setAttribute("class","fa fa-camera");
    div_info_href.appendChild(div_info_href_img);

    div_info_href.appendChild(document.createTextNode("  " + filename));

    var div_info_href_size = document.createElement("span");
    div_info_href_size.setAttribute("class","mailbox-attachment-size");
    div_info_href_size.textContent = convertFileSize(parseInt(filesize));
    div_info_href.appendChild(div_info_href_size);

    var div_info_href_dl = document.createElement("a");
    div_info_href_dl.setAttribute("href","/php/downloadfile.php?project_id="+project_id+"&imageid="+id);
    div_info_href_dl.setAttribute("class","btn btn-default btn-xs pull-right");
    div_info_href_size.appendChild(div_info_href_dl);

    var div_info_href_dl_img = document.createElement("i");
    div_info_href_dl_img.setAttribute("class","fa fa-cloud-download");
    div_info_href_dl.appendChild(div_info_href_dl_img);

    return d;
}

function convertFileSize(fileSize)
{
    var retFileSIze = '';
    if (fileSize > 1024 * 1024)
        retFileSIze = (Math.round(fileSize * 100 / (1024 * 1024)) / 100).toString() + 'MB';
    else
        retFileSIze = (Math.round(fileSize * 100 / 1024) / 100).toString() + 'KB';
    return retFileSIze;
}

function reload_surveyresults()
{
    console.log("reload_surveyresults() glReloadTimes:"+glReloadTimes);

    PreLoadIndex = 4;
    CurrentLoadIndex = 5;

    setTimeout(check_loading_status, 10000);
}

function find_survey(surveyid)
{
    for(var index=0;index<glSurvey_List.length;index++)
    {
        var survey = glSurvey_List[index];
        //var result = surveyid.localeCompare(survey.surveyid);

        // console.log("find_survey() survey.surveyid:"+survey.surveyid +" surveyid:"+surveyid +" Found");

        if ( parseInt(survey.surveyid) == parseInt(surveyid) )
        //if ( result == 0 )
        {
            // console.log("find_survey() survey:"+survey +" surveyid:"+surveyid +" Found");
            return survey;
        }
    }

    return null;
}

function load_message_status()
{
    glMessageStatus_List = [];


    var messagehashkey = '';
    for(var index=0;index<glPushNotifyMessageRecord_List.length;index++)
    {
        var record = glPushNotifyMessageRecord_List[index];
        messagehashkey = record.messagehashkey;
        break;
    }

    // console.log("load_PushNotify_uploadimage() messagehashkey:"+messagehashkey);

    if (messagehashkey.length == 0)
        return;

    var targetURL = gl_target_server + "/php/pushnotify.php";

    var querytext = 'loadmessagestatus=1'+'&project_id='+project_id+'&hashkeys='+messagehashkey+"&loadaccountname=1";

    $.getJSON(targetURL, querytext, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            glMessageStatus_List.push(record);
        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .complete(function() {

    });
}

function show_message_status()
{
    var ui_read_div = document.getElementById("message_read");
    while (ui_read_div.firstChild)
    {
        ui_read_div.removeChild(ui_read_div.firstChild);
    }

    var ui_row_div = document.createElement("div");
    ui_read_div.appendChild(ui_row_div);

    var read_count = 0;
    for(var i=0;i<glMessageStatus_List.length;i++)
    {
        var record = glMessageStatus_List[i];
        if ( record.readed == '1' )
        {
            var item_user_span = document.createElement("span");
            item_user_span.setAttribute("class","label label-success");
            item_user_span.textContent = record.accountname;
            ui_row_div.appendChild(item_user_span);

            var badge_date = document.createElement("span");
            badge_date.setAttribute("class","label label-info");
            badge_date.textContent = convertPrettyDateOnly(record.datetime, "+00:00");
            ui_row_div.appendChild(badge_date);

            var badge_time = document.createElement("span");
            badge_time.setAttribute("class","label label-warning");
            badge_time.textContent = convertPrettyTimeOnly(record.datetime, "+00:00");
            ui_row_div.appendChild(badge_time);

            var item_space_span = document.createElement("span");
            item_space_span.setAttribute("style","width:20px; display: inline-block;");
            ui_row_div.appendChild(item_space_span);

            read_count++;

            if (read_count >= 5)
            {
                var ui_row_div = document.createElement("div");
                ui_read_div.appendChild(ui_row_div);
                read_count = 0;
            }
        }
    }
}

function move_page_backward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    if ( glCurrentPage <= 1 )
    {
        // do nothing
    }
    else
    {
        glCurrentPage--;
        current_page_num_ui.textContent = glCurrentPage;
        PreLoadIndex = 0;
        CurrentLoadIndex = 1;
        check_loading_status();
    }
}

function move_page_forward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    glCurrentPage++;
    current_page_num_ui.textContent = glCurrentPage;
    PreLoadIndex = 0;
    CurrentLoadIndex = 1;
    check_loading_status();
}

function getUserNameByTopic(topic)
{
    var username = '';
    for(var index=0;index<glSurveyResult_List.length;index++)
    {
        var surveyResult = glSurveyResult_List[index];

        for(var rindex=0;rindex<surveyResult.record.length;rindex++)
        {
            var record = surveyResult.record[rindex];

            if ( topic === record.topic )
            {
                username = record.name;
                break;
            }
        }
        if (username.length > 0)
            break;
    }
    return username;
}

// function getNodeNameByTopic(topic)
// {
//     var topicList = topic.split("_");
//     var nodetype = topicList[topicList.length - 2];
//     var nodemacaddress = topicList[topicList.length - 1];
//
//     // console.log("getNodeNameByTopic() topic:"+topic +" nodetype:"+nodetype +" nodemacaddress:"+nodemacaddress);
//
//     if ( nodetype === "LOCATOR" )
//     {
//         for	(var index = 0; index < glLocators.length; index++)
//         {
//             var nodeData = glLocators[index];
//             var nodeid = nodeData[3];
//             var nodename = nodeData[5];
//             var macaddress = nodeData[6];
//
//             if ( nodemacaddress === macaddress )
//             {
//                 return nodename;
//             }
//         }
//     }
//     else
//     if ( (nodetype === "TAG") || (nodetype === "USER"))
//     {
//         for	(var index = 0; index < glTags.length; index++)
//         {
//             var nodeData = glTags[index];
//             var nodeid = nodeData[3];
//             var nodename = nodeData[5];
//             var macaddress = nodeData[6];
//
//             if ( nodemacaddress === macaddress )
//             {
//                 return nodename;
//             }
//         }
//     }
//
// }

function convertPrettyDateOnly(inTime, offset)
{
    var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss").utcOffset(offset);
    var year = cur_date.format('YYYY');
    var month = cur_date.format('M');
    var day = cur_date.format('D');

    var perty_str = "";
    perty_str = year + "年" + month + "月" + day + "日 ";
    return perty_str;
}

function convertPrettyTimeOnly(inTime, offset)
{
    var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss").utcOffset(offset);
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    var perty_str = "";
    if (hour >= 0)
        perty_str += hour + "點";
    if (minutes >= 0)
        perty_str += minutes + "分";
    if (seconds >= 0)
        perty_str += seconds + "秒";

    return perty_str;
}

function show_surveyresult(project_id, hashkey)
{
    var win = window.open('/index.php/vilssurveyresult/'+project_id+'/'+hashkey, '_blank');
    win.focus();
}

function open_surveyrecords_page()
{
    var win = window.open('/index.php/vilssurveyrecords/'+project_id, '_blank');
    win.focus();
}
