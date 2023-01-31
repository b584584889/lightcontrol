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

var gl_usermaingroups = [];
var gl_usersubgroups = [];
var glSurvey_List = [];

var glCurrentPage = 1;

var glNewSurveyIndex = 0;

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

                load_usermaingroups();
                load_usersubgroup();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_surveys();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_surveys();

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

function load_usermaingroups()
{
    gl_usermaingroups = gl_maingroups.split(",");
}

function load_usersubgroup()
{
    gl_usersubgroups = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadusersubgroup=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, usersubgroup)
        {
            gl_usersubgroups.push(usersubgroup);
        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_surveys()
{
    var targetURL = gl_target_server + "/php/survey.php";
    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    glSurvey_List = [];

    $.getJSON(targetURL, 'loadsurveys=1'+'&project_id='+project_id+"&offset="+offset+"&count="+count+"&accountid="+accountid, function(data)
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

function show_surveys()
{
    var table_body_ui = document.getElementById('SURVEY_TABLE_BODY');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    // console.log("show_surveys() glSurvey_List.length:"+glSurvey_List.length);

    for(var index=0;index<glSurvey_List.length;index++)
    {
        show_survey(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + index+1, glSurvey_List[index]);
    }
}

function show_survey(table_body_ui, ui_index, record)
{
    var cellIndex = 0;

    var row = document.createElement('tr');
    row.setAttribute("id", record.dbId+"_TABLE_ROW");
    if ((ui_index % 2) == 0)
        row.setAttribute("bgcolor","#ebebe0");
    table_body_ui.appendChild(row);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // 事項名稱
    row.appendChild(document.createElement('td'));
    var titleDiv = document.createElement('div');
    titleDiv.setAttribute("id", "title_"+record.surveyid);
    titleDiv.appendChild(document.createTextNode(record.title));
    row.cells[cellIndex].appendChild(titleDiv);
    cellIndex++;

    // 事項說明
    row.appendChild(document.createElement('td'));
    var descriptionDiv = document.createElement('div');
    descriptionDiv.setAttribute("id", "description_"+record.surveyid);
    descriptionDiv.appendChild(document.createTextNode(record.description));
    row.cells[cellIndex].appendChild(descriptionDiv);
    cellIndex++;

    // 選擇類型
    var type1 = false;
    var type2 = false;
    row.appendChild(document.createElement('td'));
    var typeDiv = document.createElement('div');
    typeDiv.setAttribute("id", "type_"+record.surveyid);
    row.cells[cellIndex].appendChild(typeDiv);

    if ( record.type === '0' )
    {
        type1 = true;
        typeDiv.appendChild(document.createTextNode("單選"));
    }
    if ( record.type === '1' )
    {
        type2 = true;
        typeDiv.appendChild(document.createTextNode("複選"));
    }
    cellIndex++;

    // 事項列表
    row.appendChild(document.createElement('td'));
    var survey_item = show_survey_item(record.surveyid, record.items);
    row.cells[cellIndex].appendChild(survey_item);
    cellIndex++;

    // 子集合權限
    var td_subgroups = create_subgroup(record.subgroups);
    td_subgroups.setAttribute("id","uiid_subgroup_"+record.surveyid);
    row.appendChild(td_subgroups);
    cellIndex++;

    // 操作
    // Edit button
    row.appendChild(document.createElement('td'));
    if ( check_editable(record.maingroups) )
    {
        var editBtn = document.createElement("button");
        editBtn.setAttribute("id", "editbtn_"+record.surveyid);
        editBtn.setAttribute("title","Edit this Survey");
        editBtn.setAttribute("href","javascript:;");
        editBtn.setAttribute("onclick","edit_survey("+ record.surveyid +");return false;");
        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("id", "editimg_"+record.surveyid);
        imgEdit.setAttribute("class","fa fa-edit");
        editBtn.appendChild(imgEdit);
        row.cells[cellIndex].appendChild(editBtn);

        // Delete button
        var deleteBtn = document.createElement("button");
        deleteBtn.setAttribute("id", "deletebtn_"+record.surveyid);
        deleteBtn.setAttribute("style","display:none");
        deleteBtn.setAttribute("title","Delete this Survey");
        deleteBtn.setAttribute("href","javascript:;");
        deleteBtn.setAttribute("onclick","confirm_delete_survey("+ record.surveyid +");return false;");
        var imgdelete = document.createElement("i");
        imgdelete.setAttribute("class","fa fa-trash-o");
        deleteBtn.appendChild(imgdelete);
        row.cells[cellIndex].appendChild(deleteBtn);
    }
    cellIndex++;
}

function show_survey_item(surveyid, items)
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
    survey_tablehead_th_0.setAttribute("style","width: 30px");
    survey_tablehead_tr.appendChild(survey_tablehead_th_0);

    var survey_tablehead_th_1 = document.createElement('th');
    survey_tablehead_tr.appendChild(survey_tablehead_th_1);

    var survey_tablehead_th_2 = document.createElement('th');
    survey_tablehead_tr.appendChild(survey_tablehead_th_2);


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
        survey_td_0.appendChild(document.createTextNode(index+1));

        var survey_td_1 = document.createElement('td');
        survey_tr.appendChild(survey_td_1);
        survey_td_1.appendChild(document.createTextNode(item.description));

        var survey_td_2 = document.createElement('td');
        survey_tr.appendChild(survey_td_2);
    }

    return survey_item;
}

function edit_survey(surveyid)
{
    var titleDiv = document.getElementById("title_"+surveyid);
    var descriptionDiv = document.getElementById("description_"+surveyid);
    var typeDiv = document.getElementById("type_"+surveyid);
    var survey_item = document.getElementById("survey_item_"+surveyid);
    var survey_tablebody = document.getElementById("survey_table_body_"+surveyid);
    var subgrouplist_span = document.getElementById("uiid_subgroup_"+surveyid);

    while (titleDiv.firstChild)
        titleDiv.removeChild(titleDiv.firstChild);
    while (descriptionDiv.firstChild)
        descriptionDiv.removeChild(descriptionDiv.firstChild);
    while (typeDiv.firstChild)
        typeDiv.removeChild(typeDiv.firstChild);
    while (survey_tablebody.firstChild)
        survey_tablebody.removeChild(survey_tablebody.firstChild);
    while (subgrouplist_span.firstChild)
        subgrouplist_span.removeChild(subgrouplist_span.firstChild);

    var editBtn = document.getElementById("editbtn_"+surveyid);
    var editImg = document.getElementById("editimg_"+surveyid);
    editBtn.setAttribute("title","Save this Survey");
    editBtn.setAttribute("onclick","save_survey("+ surveyid +");return false;");
    editImg.setAttribute("class","fa fa-save");

    var deleteBtn = document.getElementById("deletebtn_"+surveyid);
    deleteBtn.setAttribute("style","display:block");


    for(var index=0;index<glSurvey_List.length;index++)
    {
        var survey = glSurvey_List[index];
        if( survey.surveyid == surveyid )
        {
            titleDiv.appendChild(create_input_text("input_title_"+surveyid, survey.title));
            descriptionDiv.appendChild(create_input_text("input_description_"+surveyid, survey.description));
            //typeDiv.appendChild(create_input_text("type_"+surveyid, survey.type));

            var radio1_checked = false;
            var radio2_checked = false;
            if ( survey.type === '0' )
                radio1_checked = true;
            if ( survey.type === '1' )
                radio2_checked = true;
            var radio1 = create_radio_button("optionsRadios1_"+surveyid, "optionsRadios_"+surveyid, "option1", "單選", radio1_checked);
            typeDiv.appendChild(radio1);
            var radio2 = create_radio_button("optionsRadios2_"+surveyid, "optionsRadios_"+surveyid, "option2", "複選", radio2_checked);
            typeDiv.appendChild(radio2);

            for(var itemindex=0;itemindex<survey.items.length;itemindex++)
            {
                var item = survey.items[itemindex];

                var survey_tr = document.createElement('tr');
                survey_tr.setAttribute("id","surveyitem_row_"+surveyid+"_"+item.surveyitemid);
                survey_tablebody.appendChild(survey_tr);

                var survey_td_0 = document.createElement('td');
                survey_tr.appendChild(survey_td_0);
                survey_td_0.appendChild(document.createTextNode(itemindex+1));

                var survey_td_1 = document.createElement('td');
                survey_tr.appendChild(survey_td_1);

                var survey_input = create_input_text('surveyitem_'+surveyid+"_"+ item.surveyitemid, item.description);
                survey_td_1.appendChild(survey_input);

                var survey_td_2 = document.createElement('td');
                survey_tr.appendChild(survey_td_2);

                var survey_bthgroup = document.createElement('div');
                survey_bthgroup.setAttribute("class","btn-group");
                survey_td_2.appendChild(survey_bthgroup);

                var survey_button = document.createElement('button');
                survey_button.setAttribute("class","btn btn-block btn-danger");
                survey_button.setAttribute("onclick","delete_survey_item_click("+surveyid+", "+item.surveyitemid+");return true;");
                survey_bthgroup.appendChild(survey_button);

                var survey_img = document.createElement('i');
                survey_img.setAttribute("class","fa fa-minus");
                survey_button.appendChild(survey_img);


            }

            var survey_bthgroup = document.createElement('div');
            survey_bthgroup.setAttribute("class","btn-group");
            survey_item.appendChild(survey_bthgroup);

            var survey_button = document.createElement('button');
            survey_button.setAttribute("class","btn btn-block btn-success");
            survey_button.setAttribute("onclick","create_survey_item_click("+surveyid+");return true;");
            survey_bthgroup.appendChild(survey_button);

            var survey_img = document.createElement('i');
            survey_img.setAttribute("class","fa fa-plus");
            survey_button.appendChild(survey_img);

            var div_subgrouplist = create_edit_subgrouplist_ui(surveyid, survey.subgroups);
            subgrouplist_span.appendChild(div_subgrouplist);

            break;
        }
    }
}

function save_survey(surveyid)
{
    var input_title = document.getElementById("input_title_"+surveyid).value;
    var input_description = document.getElementById("input_description_"+surveyid).value;
    var optionsRadios1 = document.getElementById("optionsRadios1_"+surveyid);
    var optionsRadios2 = document.getElementById("optionsRadios2_"+surveyid);

    // console.log("input_title:"+input_title);
    // console.log("input_description:"+input_description);

    if ( input_title.length == 0 )
    {
        alert("請輸入事項名稱");
        return;
    }

    if ( input_description.length == 0 )
    {
        alert("請輸入事項說明");
        return;
    }

    var survey_type = 0;
    if ( optionsRadios1.checked )
    {
        // console.log("option:"+"單選");
        survey_type = 0;
    }
    else
    if ( optionsRadios2.checked )
    {
        // console.log("option:"+"複選");
        survey_type = 1;
    }

    var curSurvey = undefined;
    for(var index=0;index<glSurvey_List.length;index++)
    {
        var survey = glSurvey_List[index];
        if( survey.surveyid == surveyid )
        {
            curSurvey = survey;
            break;
        }
    }

    if ( curSurvey == undefined )
    {
        alert("無此事項紀錄!");
        return;
    }


    var surveys = [];

    for(var index=0;index<curSurvey.items.length;index++)
    {
        var item = curSurvey.items[index];

        var itemui = document.getElementById("surveyitem_"+surveyid+"_"+item.surveyitemid);
        if (itemui == undefined)
        {
            itemui = document.getElementById("surveyitem_"+surveyid+"_new_"+item.surveyitemnewid);
            if (itemui == undefined)
                break;
        }

        item['description'] = itemui.value;
        // console.log("surveyitem:"+index+" surveyitemid:"+item.surveyitemid+" surveyitemnewid:"+item.surveyitemnewid+" description:"+item.description+" DateTime:"+item.DateTime);

        surveys.push(item);
    }
    // console.log("surveys.length:"+surveys.length);

    var subgroups = getSelectedSubgroup(surveyid);

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_survey'] = 1;
    jsondata['surveyid'] = surveyid;
    jsondata['title'] = input_title;
    jsondata['description'] = input_description;
    jsondata['type'] = survey_type;
    jsondata['items'] = surveys;
    jsondata['accountid'] = accountid;
    jsondata['subgroups'] = subgroups;

    var targetURL = gl_target_server + "/php/survey.php";

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

            setTimeout(reload_page, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });

}

function save_new_survey()
{
    var input_title = document.getElementById("input_title").value;
    var input_description = document.getElementById("input_description").value;
    var optionsRadios1 = document.getElementById("optionsRadios1");
    var optionsRadios2 = document.getElementById("optionsRadios2");

    var subgroups = getSelectedSubgroup('new');

    console.log("save_new_survey() subgroups:"+subgroups);
    // console.log("input_title:"+input_title);
    // console.log("input_description:"+input_description);

    if ( input_title.length == 0 )
    {
        alert("請輸入事項名稱");
        return;
    }

    if ( input_description.length == 0 )
    {
        alert("請輸入事項說明");
        return;
    }

    var survey_type = 0;
    if ( optionsRadios1.checked )
    {
        // console.log("option:"+"單選");
        survey_type = 0;
    }
    else
    if ( optionsRadios2.checked )
    {
        // console.log("option:"+"複選");
        survey_type = 1;
    }

    var surveys = [];

    for(var index=1;index<=glNewSurveyIndex;index++)
    {
        var surveyitem = document.getElementById("surveyitem_" + index).value;
        // console.log("surveyitem:"+index+" "+surveyitem);

        surveys.push(surveyitem);
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_new_survey'] = 1;
    jsondata['title'] = input_title;
    jsondata['description'] = input_description;
    jsondata['type'] = survey_type;
    jsondata['items'] = surveys;
    jsondata['accountid'] = accountid;
    jsondata['subgroups'] = subgroups;

    var targetURL = gl_target_server + "/php/survey.php";

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

            setTimeout(reload_page, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });

}

function confirm_delete_survey(surveyid)
{
    if (confirm('Are you sure you want to delete this survey ?'))
    {
        // delete it!
        delete_survey(surveyid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_survey(surveyid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_survey'] = 1;
    jsondata['surveyid'] = surveyid;
    jsondata['accountid'] = accountid;

    var targetURL = gl_target_server + "/php/survey.php";

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

            setTimeout(reload_page, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function create_empty_survey_table_item(ui_index)
{
    var record_table = document.getElementById('SURVEY_TABLE_BODY');
    var input_title = document.getElementById("input_title");
    if (input_title != undefined)
        return;

    var row = record_table.insertRow(0);
    //var row = document.createElement('tr');
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""));
    cellIndex++;

    // 事項名稱
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_title"));
    cellIndex++;

    // 事項說明
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_description"));
    cellIndex++;

    // 選擇類型
    row.appendChild(document.createElement('td'));
    var radio1 = create_radio_button("optionsRadios1", "optionsRadios", "option1", "單選", true);
    row.cells[cellIndex].appendChild(radio1);
    var radio2 = create_radio_button("optionsRadios2", "optionsRadios", "option2", "複選", false);
    row.cells[cellIndex].appendChild(radio2);
    cellIndex++;

    // 事項列表
    row.appendChild(document.createElement('td'));
    var survey_item = create_survey_item();
    row.cells[cellIndex].appendChild(survey_item);
    cellIndex++;

    // Subgroup list
    row.appendChild(document.createElement('td'));
    var div_subgrouplist = create_edit_subgrouplist_ui("new", "");
    row.cells[cellIndex].appendChild(div_subgrouplist);
    cellIndex++;


    // save button
    row.appendChild(document.createElement('td'));
    var Button_save = document.createElement("button");
    Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
    Button_save.setAttribute("onclick","save_new_survey();");
    Button_save.textContent = "儲存";
    row.cells[cellIndex].appendChild(Button_save);
    cellIndex++;

    // record_table.appendChild(row);
}

function create_input_text(ui_id, text)
{
    var input_text = document.createElement("textarea");
    input_text.setAttribute("rows","1");
    input_text.setAttribute("id",ui_id);
    input_text.textContent = text;

    return input_text;
}

function create_subgroup_label(context)
{
    var subgroup_span = document.createElement("span");
    subgroup_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    subgroup_span.textContent = context;
    return subgroup_span;
}

function create_empty_spcae()
{
    var Spanspace = document.createElement("span");
    Spanspace.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;");
    Spanspace.textContent = "  ";
    return Spanspace;
}

function create_radio_button(ui_id, name, value, text, checked)
{
    var radioBtn = document.createElement('div');
    radioBtn.setAttribute("class","radio");

    var radio_label = document.createElement('label');
    radioBtn.appendChild(radio_label);
    var radio_input = document.createElement('input');
    radio_input.setAttribute("type","radio");
    radio_input.setAttribute("id",ui_id);
    radio_input.setAttribute("name",name);
    radio_input.setAttribute("value",value);
    radio_label.appendChild(radio_input);
    radio_label.appendChild(document.createTextNode(text));

    if (checked)
        radio_input.checked = true;

    return radioBtn;
}

function create_survey_item()
{
    glNewSurveyIndex = 1;

    var survey_item = document.createElement('div');

    var survey_table = document.createElement('table');
    survey_item.appendChild(survey_table);

    var survey_tablehead = document.createElement('thead');
    survey_table.appendChild(survey_tablehead);

    var survey_tablehead_tr = document.createElement('tr');
    survey_tablehead.appendChild(survey_tablehead_tr);

    var survey_tablehead_th_0 = document.createElement('th');
    survey_tablehead_th_0.setAttribute("style","width: 30px");
    survey_tablehead_tr.appendChild(survey_tablehead_th_0);

    var survey_tablehead_th_1 = document.createElement('th');
    //survey_tablehead_th_1.setAttribute("style","width: 30px");
    survey_tablehead_tr.appendChild(survey_tablehead_th_1);


    var survey_tablebody = document.createElement('tbody');
    survey_tablebody.setAttribute("id","new_survey_table_body");
    survey_table.appendChild(survey_tablebody);

    var survey_tr = document.createElement('tr');
    survey_tablebody.appendChild(survey_tr);

    var survey_td_0 = document.createElement('td');
    survey_tr.appendChild(survey_td_0);
    survey_td_0.appendChild(document.createTextNode(glNewSurveyIndex));

    var survey_td_1 = document.createElement('td');
    survey_tr.appendChild(survey_td_1);

    var survey_input = create_input_text('surveyitem_' + glNewSurveyIndex, "");
    survey_td_1.appendChild(survey_input);

    var survey_bthgroup = document.createElement('div');
    survey_bthgroup.setAttribute("class","btn-group");
    survey_item.appendChild(survey_bthgroup);

    var survey_button = document.createElement('button');
    survey_button.setAttribute("class","btn btn-block btn-success btn-lg");
    survey_button.setAttribute("onclick","createnewsurvey_item_click();return true;");
    survey_bthgroup.appendChild(survey_button);

    var survey_img = document.createElement('i');
    survey_img.setAttribute("class","fa fa-plus");
    survey_button.appendChild(survey_img);

    return survey_item;
}

function createnewsurvey_item_click()
{
    glNewSurveyIndex++;

    console.log("createnewsurvey_item_click() glNewSurveyIndex:"+glNewSurveyIndex);

    var survey_tablebody = document.getElementById('new_survey_table_body');

    var survey_tr = document.createElement('tr');
    survey_tablebody.appendChild(survey_tr);

    var survey_td_0 = document.createElement('td');
    survey_tr.appendChild(survey_td_0);
    survey_td_0.appendChild(document.createTextNode(glNewSurveyIndex));

    var survey_td_1 = document.createElement('td');
    survey_tr.appendChild(survey_td_1);

    var survey_input = create_input_text('surveyitem_' + glNewSurveyIndex, "");
    survey_td_1.appendChild(survey_input);

    return false;
}

function create_survey_item_click(surveyid)
{
    var survey_tablebody = document.getElementById("survey_table_body_"+surveyid);

    for(var index=0;index<glSurvey_List.length;index++)
    {
        var survey = glSurvey_List[index];
        if( survey.surveyid == surveyid )
        {
            var new_itemindex = survey.items.length;
            var item = {};
            item['surveyitemid'] = -1;
            item['surveyitemnewid'] = new_itemindex;
            item['surveyitemdelete'] = 0;
            item['description'] = "";
            item['DateTime'] = "";
            survey.items.push(item);

            var survey_tr = document.createElement('tr');
            survey_tablebody.appendChild(survey_tr);

            var survey_td_0 = document.createElement('td');
            survey_tr.appendChild(survey_td_0);
            survey_td_0.appendChild(document.createTextNode(new_itemindex+1));

            var survey_td_1 = document.createElement('td');
            survey_tr.appendChild(survey_td_1);

            var survey_input = create_input_text('surveyitem_'+surveyid+"_new_"+ new_itemindex, item.description);
            survey_td_1.appendChild(survey_input);

            break;
        }
    }
}

function delete_survey_item_click(surveyid, surveyitemid)
{
    for(var index=0;index<glSurvey_List.length;index++)
    {
        var survey = glSurvey_List[index];
        if( survey.surveyid == surveyid )
        {
            for(var itemindex=0;itemindex<survey.items.length;itemindex++)
            {
                var item = survey.items[itemindex];
                if (item.surveyitemid == surveyitemid)
                {
                    var survey_tr = document.getElementById("surveyitem_row_"+surveyid+"_"+surveyitemid);
                    survey_tr.setAttribute("style","display:none");

                    item['surveyitemdelete'] = 1;
                    survey.items[itemindex] = item;
                    break;
                }
            }
            // var new_itemindex = survey.items.length;
            // var item = {};
            // item['surveyitemid'] = -1;
            // item['surveyitemnewid'] = new_itemindex;
            // item['description'] = "";
            // item['DateTime'] = "";
            // survey.items.push(item);
            //
            // var survey_tr = document.createElement('tr');
            // survey_tablebody.appendChild(survey_tr);
            //
            // var survey_td_0 = document.createElement('td');
            // survey_tr.appendChild(survey_td_0);
            // survey_td_0.appendChild(document.createTextNode(new_itemindex+1));
            //
            // var survey_td_1 = document.createElement('td');
            // survey_tr.appendChild(survey_td_1);
            //
            // var survey_input = create_input_text('surveyitem_'+surveyid+"_new_"+ new_itemindex, item.description);
            // survey_td_1.appendChild(survey_input);

            break;
        }
    }
}

function create_edit_subgrouplist_ui(fileid, idlist)
{
    var div_subgrouplist = document.createElement("div");
    div_subgrouplist.setAttribute("class","btn-group");

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    div_button.textContent = "子集合";
    div_subgrouplist.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_subgrouplist.appendChild(div_ul);

    // select all button
    if ( gl_groupid == 2 || gl_groupid == 3 )
    {
        var item_li = document.createElement("li");
        var item_click = document.createElement("a");
        item_click.setAttribute("class","small");
        item_click.setAttribute("href","javascript:;");
        item_click.setAttribute("onclick","dropdown_selectall(\"subgroup\",\""+fileid+"\");return true;");
        item_li.appendChild(item_click);
        item_click.appendChild(document.createTextNode("全選"));
        div_ul.appendChild(item_li);
    }

    var idList = idlist.split(",");

    for (var i = 0; i < gl_usersubgroups.length; ++i)
    {
        var usersubgroup = gl_usersubgroups[i];
        var existed = check_numer_in_list(parseInt(usersubgroup.subgroupid), idList);
        div_ul.appendChild(create_dropdown_subgroup_item(fileid, i, parseInt(usersubgroup.subgroupid), usersubgroup.subgroupname, existed));
    }

    return div_subgrouplist;
}

function create_dropdown_subgroup_item(fileid, index, datavalue, dataname, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", datavalue);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_subgroup_click(\""+fileid+"\","+ index +");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_subgroup_"+fileid+"_"+index);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_subgroup_click(\""+fileid+"\","+ index +");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+dataname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_checkbox_subgroup_click(fileid, index)
{
    glDropdown_checkbox_subgroup_click = true;
}

var glDropdown_checkbox_subgroup_click = false;
function dropdown_subgroup_click(fileid, index)
{
    if (glDropdown_checkbox_subgroup_click == true)
    {
        glDropdown_checkbox_subgroup_click = false;
        return;
    }

    if (document.getElementById("dropdown_checkbox_subgroup_"+fileid+"_"+index).checked == true)
        document.getElementById("dropdown_checkbox_subgroup_"+fileid+"_"+index).checked = false;
    else
        document.getElementById("dropdown_checkbox_subgroup_"+fileid+"_"+index).checked = true;

    glDropdown_checkbox_subgroup_click = false;
}

function dropdown_selectall(prefix, fileid)
{
    console.log("dropdown_selectall() prefix:"+prefix+" fileid:"+fileid);

    for(var i=0;i<100;i++)
    {
        var ele = document.getElementById("dropdown_checkbox_"+prefix+"_"+fileid+"_"+i);
        if ( ele == undefined )
        {
            console.log("dropdown_selectall() fileid:"+fileid+" i:"+i+" ele == undefined ");
            break;
        }
        ele.checked = true;
    }
}

function create_subgroup(subgroups)
{
    var d = document.createElement("td");

    var subgroupList = subgroups.split(",");

    if (subgroupList.indexOf('99999') >= 0)
    {
        var subgroupL_div = document.createElement("div");
        subgroupL_div.appendChild(create_subgroup_label('全部'));
        d.appendChild(subgroupL_div);
    }
    else
    {
        var count = 0;
        var subgroupL_div = undefined;
        for(var index=0;index<subgroupList.length;index++)
        {
            var subgroupid = subgroupList[index];
            var SubgroupName = getSubgroupName(subgroupid);
            if (SubgroupName.length > 0)
            {
                if ( count == 0 )
                {
                    subgroupL_div = document.createElement("div");
                    d.appendChild(subgroupL_div);
                }

                subgroupL_div.appendChild(create_subgroup_label(SubgroupName));
                subgroupL_div.appendChild(create_empty_spcae());

                count++;
                if ( count >= 5 )
                {
                    count = 0;
                }
            }
        }
    }

    return d;
}

function getSelectedSubgroup(fileid)
{
    var subgroups = [];
    var count = 0;
    for (var index = 0; index < gl_usersubgroups.length; ++index)
    {
        var usersubgroup = gl_usersubgroups[index];

        if (document.getElementById("dropdown_checkbox_subgroup_"+fileid+"_"+index).checked)
        {
            var subgroupid = parseInt(usersubgroup.subgroupid);
            subgroups.push(subgroupid);
            count++;
        }
    }

    if ( gl_groupid == 1 || gl_groupid == 2 )
    {
        if ( count == gl_usersubgroups.length )
        {
            subgroups = [];
            subgroups.push(99999);
        }
    }

    return subgroups.toString();
}

function check_numer_in_list(targetNumber, dataList)
{
    var existed = false;
    for (var i = 0; i < dataList.length; ++i)
    {
        var data = dataList[i];

        if ( parseInt(data) == targetNumber || parseInt(data) == 99999 )
        {
            existed = true;
            break;
        }
    }

    return existed;
}

function check_editable(maingroups)
{
    var found = false;
    var usermaingroups = maingroups.split(",");

    for (var i = 0; i < gl_usermaingroups.length; ++i)
    {
        var mymaingroup = gl_usermaingroups[i];

        for (var j = 0; j < usermaingroups.length; ++j)
        {
            var surveymaingroup = usermaingroups[j];

            if ( mymaingroup == surveymaingroup )
            {
                found = true;
                break;
            }
        }

        if (found)
        {
            break;
        }
    }

    return found;
}

function getSubgroupName(subgroupid)
{
    var subgroupname = '';

    if ( subgroupid == '99999' )
    {
        return '全部';
    }

    for (var i = 0; i < gl_usersubgroups.length; ++i)
    {
        var subgroup = gl_usersubgroups[i];

        // console.log("getSubgroupName() subgroup.subgroupid:"+subgroup.subgroupid);

        if ( parseInt(subgroup.subgroupid) == subgroupid )
        {
            subgroupname = subgroup.subgroupname;
            break;
        }

    }
    return subgroupname;
}

function create_new_survey()
{
    create_empty_survey_table_item(glSurvey_List.length + 1);

    return false;
}

// $("#createnewsurveybutton").click(function()
// {
//     create_empty_survey_table_item(glSurvey_List.length + 1);
//
//     return false;
// });

function reload_page()
{
    PreLoadIndex = -1;
    CurrentLoadIndex = 0;
    check_loading_status();
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
        reload_page();
    }
}

function move_page_forward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    glCurrentPage++;
    current_page_num_ui.textContent = glCurrentPage;
    reload_page();
}

function open_survey_page()
{
    var win = window.open('/index.php/vilssurvey/'+project_id, '_blank');
    win.focus();
}
