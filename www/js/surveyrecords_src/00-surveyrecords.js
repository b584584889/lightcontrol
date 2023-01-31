/*
 * Author: ikki Chung
 * Date: 2020 04 06
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var LOAD_PAGE_NUM = 20;

var glSurvey_List = [];
var glSurveyEvent_List = [];
var glSurveyResult_List = [];

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
                load_surveys();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                load_surveyevents();
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
                load_surveyresults();

            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                show_surveyresults();
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

function load_surveys()
{
    var targetURL = gl_target_server + "/php/survey.php";

    glSurvey_List = [];

    $.getJSON(targetURL, 'loadsurveys=1'+'&project_id='+project_id+'&accountid='+accountid, function(data)
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

function load_surveyevents()
{
    var targetURL = gl_target_server + "/php/survey.php";
    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    glSurveyEvent_List = [];

    $.getJSON(targetURL, 'loadsurveyevents=1'+'&project_id='+project_id+"&offset="+offset+"&count="+count+'&accountid='+accountid, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            glSurveyEvent_List.push(record);
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

    $.getJSON(targetURL, 'loadsurveyresults=1'+'&project_id='+project_id+"&hashkeys="+hashkeys, function(data)
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

    console.log("show_surveyevents() glSurveyEvent_List.length:"+glSurveyEvent_List.length);

    for(var index=0;index<glSurveyEvent_List.length;index++)
    {
        show_surveyevent(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + index+1, glSurveyEvent_List[index]);
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
    row.appendChild(document.createElement('td'));
    var survey_item = show_survey_item(survey.surveyid, survey.items, record.hashkey);
    row.cells[cellIndex].appendChild(survey_item);
    cellIndex++;

    // deadline
    row.appendChild(document.createElement('td'));
    var badge_date = document.createElement("span");
    badge_date.setAttribute("class","label label-info");
    badge_date.textContent = record.deadline;
    row.cells[cellIndex].appendChild(badge_date);
    cellIndex++;

    // 推播時間
    row.appendChild(document.createElement('td'));
    var badge_date = document.createElement("span");
    badge_date.setAttribute("class","label label-success");
    badge_date.textContent = record.DateTime;
    row.cells[cellIndex].appendChild(badge_date);
    cellIndex++;

    // 推播紀錄id
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+record.PushNotifyMessageRecorddbId));
    cellIndex++;

    // 操作
    row.appendChild(document.createElement('td'));

    var checkBtn = document.createElement("button");
    checkBtn.setAttribute("title","查看詳細資訊");
    checkBtn.setAttribute("href","javascript:;");
    checkBtn.setAttribute("onclick","show_surveyresult("+ project_id +",\""+record.surveyid+"\",\""+record.hashkey+"\");return false;");
    var imgCheck = document.createElement("i");
    imgCheck.setAttribute("class","fa fa-calendar-check-o ");
    checkBtn.appendChild(imgCheck);
    row.cells[cellIndex].appendChild(checkBtn);

    cellIndex++;

    return 1;
}

function show_survey_item(surveyid, items, hashkey)
{
    var survey_item = document.createElement('div');
    survey_item.setAttribute("id","survey_item_"+surveyid);

    var survey_table = document.createElement('table');
    survey_item.appendChild(survey_table);

    var survey_tablehead = document.createElement('thead');
    survey_table.appendChild(survey_tablehead);

    var survey_tablehead_tr = document.createElement('tr');
    survey_tablehead.appendChild(survey_tablehead_tr);

    var survey_tablehead_th_0 = document.createElement('th');
    survey_tablehead_th_0.setAttribute("style","width: 40px");
    survey_tablehead_tr.appendChild(survey_tablehead_th_0);

    var survey_tablehead_th_1 = document.createElement('th');
    survey_tablehead_tr.appendChild(survey_tablehead_th_1);

    // var survey_tablehead_th_2 = document.createElement('th');
    // survey_tablehead_tr.appendChild(survey_tablehead_th_2);

    var survey_tablebody = document.createElement('tbody');
    survey_tablebody.setAttribute("id","survey_table_body_"+surveyid);
    survey_table.appendChild(survey_tablebody);

    for(var index=0;index<items.length;index++)
    {
        var item = items[index];
        var survey_tr = document.createElement('tr');
        survey_tablebody.appendChild(survey_tr);

        var survey_td_0 = document.createElement('td');
        survey_tr.appendChild(survey_td_0);

        var survey_td_0_span = document.createElement("span");
        survey_td_0_span.setAttribute("class","label label-danger");
        survey_td_0_span.setAttribute("id","survey_item_"+hashkey+"_"+index);
        survey_td_0_span.textContent = '0';
        survey_td_0.appendChild(survey_td_0_span);

        //console.log("id:survey_item_"+hashkey+"_"+index);

        var survey_td_1 = document.createElement('td');
        survey_tr.appendChild(survey_td_1);
        survey_td_1.appendChild(document.createTextNode(item.description));

        // var survey_td_2 = document.createElement('td');
        // survey_tr.appendChild(survey_td_2);
    }

    // comment
    var survey_tr = document.createElement('tr');
    survey_tablebody.appendChild(survey_tr);

    var survey_td_0 = document.createElement('td');
    survey_tr.appendChild(survey_td_0);

    survey_td_0.appendChild(document.createTextNode("說明:"));

    // var survey_td_1 = document.createElement('td');
    // survey_tr.appendChild(survey_td_1);

    // var survey_td_2 = document.createElement('td');
    // survey_tr.appendChild(survey_td_2);

    var survey_td_comment = document.createElement('td');
    survey_td_comment.setAttribute("id","survey_item_comment_"+hashkey);
    survey_tr.appendChild(survey_td_comment);

    return survey_item;
}

function show_surveyresults()
{
    //console.log("show_surveyresults() glSurveyResult_List.length:"+glSurveyResult_List.length);

    for(var index=0;index<glSurveyResult_List.length;index++)
    {
        var surveyResult = glSurveyResult_List[index];
        //console.log("show_surveyresults() hashkey:"+surveyResult.hashkey +" surveyResult.record.length:"+surveyResult.record.length);

        var UserResults = [];
        var AllResults = [];
        var AllComments = [];

        for(var rindex=0;rindex<surveyResult.record.length;rindex++)
        {
            var record = surveyResult.record[rindex];
            var result = record.results.split(",");

            if ( record.comment.length > 0 )
                AllComments.push(record.comment);

            // if ( (AllComments.length > 0) && (record.comment.length > 0) )
            // {
            //     AllComments = AllComments + ",";
            // }
            //
            // AllComments = AllComments + record.comment;

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
            }
        }
        //console.log("show_surveyresults() AllResults.length:"+AllResults.length);


        for(var rindex=0;rindex<AllResults.length;rindex++)
        {
            //console.log("show_surveyresults() rindex:"+rindex+" AllResults="+AllResults[rindex]);
            //console.log("show_surveyresults() id:survey_item_"+surveyResult.hashkey+"_"+rindex);

            var survey_td_0_span = document.getElementById("survey_item_"+surveyResult.hashkey+"_"+rindex);
            if (survey_td_0_span != undefined)
            {
                survey_td_0_span.textContent = AllResults[rindex];
                if (AllResults[rindex] == 0)
                {
                    survey_td_0_span.setAttribute("class","label label-danger");
                }
                else
                {
                    survey_td_0_span.setAttribute("class","label label-info");
                }
            }
        }

        var survey_td_comment = document.getElementById("survey_item_comment_"+surveyResult.hashkey);
        if ( survey_td_comment == null )
        {
            continue;
        }

        while (survey_td_comment.firstChild)
        {
            survey_td_comment.removeChild(survey_td_comment.firstChild);
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

    if (glReloadTimes < 5)
    {
        setTimeout(reload_surveyresults, 10000);
        glReloadTimes++;
    }
}

function reload_surveyresults()
{
    console.log("reload_surveyresults() glReloadTimes:"+glReloadTimes);

    PreLoadIndex = 2;
    CurrentLoadIndex = 3;

    setTimeout(check_loading_status, 10000);
}

function find_survey(surveyid)
{
    for(var index=0;index<glSurvey_List.length;index++)
    {
        var survey = glSurvey_List[index];
        var result = surveyid.localeCompare(survey.surveyid);

        //if ( survey.surveyid === surveyid )
        if ( result == 0 )
        {
            return survey;
        }
    }

    return null;
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

function show_surveyresult(project_id, surveyid, hashkey)
{
    var win = window.open('/index.php/vilssurveyresult/'+project_id+'?surveyid='+surveyid+'&hashkey='+hashkey, '_blank');
    win.focus();
}

function open_surveyrecords_page()
{
    var win = window.open('/index.php/vilssurveyrecords/'+project_id, '_blank');
    win.focus();
}
