/*
 * Author: ikki Chung
 * Date: 2020.12.25
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_userregister_NUM = 20;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var glCurrentPage = 1;

var gl_userregister = [];

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

                gl_userregister = [];
                load_userregister_info();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                draw_table();

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

function reload_userregister_info()
{
    PreLoadIndex = -1;
    CurrentLoadIndex = 0;
    check_loading_status();
}

function load_userregister_info()
{
    var offset = (glCurrentPage - 1) * LOAD_userregister_NUM;
    var count = LOAD_userregister_NUM;

    gl_userregister = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";

    var p = $.getJSON(targetURL, 'loaduserregister=1&s='+offset+'&count='+count+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, userregister)
        {
            gl_userregister.push(userregister);
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

function draw_table()
{
    var record_table = document.getElementById('userregister_TABLE');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

     console.log("draw_table() gl_userregister.length:"+gl_userregister.length);

    for (var i = 0; i < gl_userregister.length; ++i)
    {
        var register = gl_userregister[i];

        var row = create_record_table_item(i, register);
        record_table.appendChild(row);
    }
}

function create_record_table_item(ui_index, record)
{
    if (record == null)
        return null;

    var row = document.createElement('tr');
    if ((ui_index % 2) == 1)
        row.setAttribute("bgcolor","#ebebe0");

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+(ui_index + 1)));
    cellIndex++;

    // 名稱
    var tdgroupid = document.createElement('td');
    row.appendChild(tdgroupid);

    var Spangroupid = create_subgroup_label(record.subgroupname);
    row.cells[cellIndex].appendChild(Spangroupid);
    cellIndex++;

    // email
    var tdemail = document.createElement('td');
    tdemail.textContent = record.email;
    row.appendChild(tdemail);
    cellIndex++;

    // send
    var tdsend = document.createElement('td');
    tdsend.textContent = record.send;
    row.appendChild(tdsend);
    cellIndex++;

    // valid
    var tdvalid = document.createElement('td');
    row.appendChild(tdvalid);

    if (record.valid == '0')
    {
        var Spanvalid = create_not_valid_label('未通過');
        row.cells[cellIndex].appendChild(Spanvalid);
    }
    else
    {
        var Spanvalid = create_valid_label('通過');
        row.cells[cellIndex].appendChild(Spanvalid);
    }
    cellIndex++;

    // datetime
    var tdsend = document.createElement('td');
    tdsend.textContent = record.datetime;
    row.appendChild(tdsend);
    cellIndex++;

    var tdoperation = document.createElement('td');
    row.appendChild(tdoperation);

    if (config_enable_edit)
    {

        // Edit button
        var editBtn = document.createElement("button");
        editBtn.setAttribute("id","uiid_editbtn_"+record.email);
        editBtn.setAttribute("title","設定 註冊通過");
        editBtn.setAttribute("href","javascript:;");
        editBtn.setAttribute("onclick","confirm_userregister(\""+record.email+"\", \""+ record.subgroupname +"\");return false;");
        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("id","uiid_editimg_"+record.email);
        imgEdit.setAttribute("class","fa fa-edit");
        editBtn.appendChild(imgEdit);
        row.cells[cellIndex].appendChild(editBtn);

        // delete uesr button
        var deleteBtn = document.createElement("button");
        // deleteBtn.setAttribute("style","display:none");
        deleteBtn.setAttribute("id","uiid_deletebtn_"+record.email);
        deleteBtn.setAttribute("title","刪除 註冊資料");
        deleteBtn.setAttribute("href","javascript:;");
        deleteBtn.setAttribute("onclick","confirm_delete_userregister(\""+record.email+"\", \""+ record.subgroupname +"\");return false;");
        var imgDelete = document.createElement("i");
        imgDelete.setAttribute("class","fa fa-user-times");
        deleteBtn.appendChild(imgDelete);
        row.cells[cellIndex].appendChild(deleteBtn);

        cellIndex++;
    }

    return row;
}

function clearChildView(uiid)
{
    var ui_item = document.getElementById(uiid);
    if ( ui_item != null )
    {
        while (ui_item.firstChild)
        {
            ui_item.removeChild(ui_item.firstChild);
        }
    }
    return ui_item;
}

function create_subgroup_label(context)
{
    var subgroup_span = document.createElement("span");
    subgroup_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    subgroup_span.textContent = context;
    return subgroup_span;
}

function create_valid_label(context)
{
    var account_span = document.createElement("span");
    account_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(54, 127, 169);");
    account_span.textContent = context;
    return account_span;
}

function create_not_valid_label(context)
{
    var account_span = document.createElement("span");
    account_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(169, 20, 54);");
    account_span.textContent = context;
    return account_span;
}

function confirm_userregister(email, groupname)
{
    if (confirm('確認通過此註冊帳號:'+email+' 主集合:'+groupname+' ?'))
    {
        // register it!
        register_userregister(email);
    }
    else
    {
        // Do nothing!
    }
}

function register_userregister(email)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['register_userregister'] = 1;
    jsondata['email'] = email;

    var targetURL = gl_target_server + "/php/vilsnodes.php";

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

            reload_userregister_info();
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_delete_userregister(email, groupname)
{
    if (confirm('確認刪除此帳號:'+email+' 主集合:'+groupname+' ?'))
    {
        // reset it!
        delete_userregister(email);
    }
    else
    {
        // Do nothing!
    }
}

function delete_userregister(email)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_userregister'] = 1;
    jsondata['email'] = email;

    var targetURL = gl_target_server + "/php/vilsnodes.php";

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

            reload_userregister_info();
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
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

        reload_userregister_info();
    }
}

function move_page_forward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    glCurrentPage++;
    current_page_num_ui.textContent = glCurrentPage;

    reload_userregister_info();

}

function open_userregister_page()
{
    var win = window.open('/index.php/vilsuserregister/'+project_id, '_blank');
    win.focus();
}
