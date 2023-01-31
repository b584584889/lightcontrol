var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var gl_getwaymac = '';

var gl_light_list = [];

var gl_firstlload = true;

var gl_situationTarget = '';
// var gl_brightMove = false;
var gl_mode = 'up';
var gl_range = 20;
var gl_moveval = 135;
var gl_gatewaymacaddr = '02:81:76:55:8d:19';    //AUO Demo Room
// var gl_gatewaymacaddr = '02:81:c0:b8:a1:3d';    //Norgay Taipei

check_loading_status();

function check_loading_status()
{

  switch (CurrentLoadIndex) {
    case 0: //群組
      if (PreLoadIndex != CurrentLoadIndex)
      {
        console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
        PreLoadIndex = CurrentLoadIndex;
        load_usergroup();
      }
      break;
    case 1: //子集合
      if (PreLoadIndex != CurrentLoadIndex)
      {
        console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
        PreLoadIndex = CurrentLoadIndex;
        load_userdatasubgroup();
      }
      break;
    case 2: //載入PM清單
      if (PreLoadIndex != CurrentLoadIndex)
      {
        console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
        PreLoadIndex = CurrentLoadIndex;

        load_light_list();
        CurrentLoadIndex++;
      }
      break;
    case 3: //載入資料
      if (PreLoadIndex != CurrentLoadIndex)
      {
        console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
        PreLoadIndex = CurrentLoadIndex;

        // load_light_list();
        CurrentLoadIndex++;
      }
      break;
    case 4: //結束載入階段
      if (PreLoadIndex != CurrentLoadIndex)
      {
        console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
        PreLoadIndex = CurrentLoadIndex;

        CurrentLoadIndex = LOAD_STATE_END;
      }
      break;
  }

  if (CurrentLoadIndex != LOAD_STATE_END)
    setTimeout(check_loading_status, 100);
  else
    console.log("check_loading_status() Stop.");

}
//使用群組
function load_usergroup()
{
  gl_usergroups = [];
  var targetURL = gl_target_server + "/php/vilsnodes.php";
  var p = $.getJSON(targetURL, 'loadusergroup=1&project_id='+project_id, function(data)
  {
      $.each(data, function(index, group)
      {
          group.groupid = parseInt(group.groupid);
          gl_usergroups.push(group);
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
//子集合
function load_userdatasubgroup()
{
  gl_userdatasubgroups = [];

  var targetURL = gl_target_server + '/php/vilsnodes.php';
  var p = $.getJSON(targetURL, 'loaddatasubgroup=1&project_id='+project_id+'&type=0', function(data)
  {
      $.each(data, function(index, group)
      {
          group.subgroupid = parseInt(group.subgroupid);
          gl_userdatasubgroups.push(group);
      });
  })
  .success(() => {
    CurrentLoadIndex = CurrentLoadIndex + 1;
  })
  .error(() => {
  })
  .complete(() => {
  });
}

function load_light_list()
{
    gl_light_list = [];
    var targetUrl = gl_target_server + "/php/lightdevicecontrol.php";
    var p = $.getJSON(targetUrl, `loadlightid&project_id=${project_id}`, function (data)
    {
        $.each(data, (index, ev) => {
            gl_light_list.push(ev);
        })
    })
    .success(() => {
        create_light_list();
    })
    .error(err => {
        console.log(`lightdevicecontrol -> load_light_list: ${err} `);
    })
    .complete(() => {

    });
}

function create_light_list()
{
    var idx = 0;
    $('#ls_list_content tr').remove();
    $.each(gl_light_list, (index, data) => {
        idx = index + 1;
        $('#ls_list_content').append(create_light_list_item(idx, data));
        if (gl_firstlload)
        {
            ChangeLightDevice(data['key']);
            gl_firstlload = false;
        }
    })
}

function create_light_list_item(index, data)
{
    var tr, td, span;

    tr = document.createElement('tr');

    td = document.createElement('td');
    td.setAttribute('class', 'listItem listIndex' + (gl_firstlload ? ' listItemSelected ' : ''));
    td.setAttribute('data-field', 'index');
    td.setAttribute('data-key', data['key']);
    td.setAttribute('data-id', data['id']);
    td.setAttribute('data-uniAddress', data['uniAddress']);
    td.textContent = index;
    tr.appendChild(td);

    td = document.createElement('td');
    td.setAttribute('class', 'listItem listData' + (gl_firstlload ? ' listItemSelected ' : ''));
    td.setAttribute('data-field', 'data');
    td.setAttribute('data-key', data['key']);
    td.setAttribute('data-id', data['id']);
    td.setAttribute('data-uniAddress', data['uniAddress']);
    tr.appendChild(td);

    span = document.createElement('td');
    span.setAttribute('class', 'badge bg-aqua-active');
    span.textContent = data['name'];
    td.appendChild(span);

    return tr;
}

function SendLightDeviceChange(type)
{
    var power = false;
    var brightness = $('ui_brightness').val();
    var colorTemperature = $('ui_colorTemperature').val();

    var state = {};

    switch (type) {
        case 'onoff':
            state['onOff'] = 0; //(power ? 1 : 0);
            break;
        case 'level':
            state['level'] = brightness;
            break;
        case 'cct':
            state['cct'] = colorTemperature;
            break;
    }

    var device = {};
    device['id'] = '2';
    device['uniAddress'] = '8';
    device['state'] = state;

    var sendString = {};
    sendString['devices'] = device;
    sendString['gateway_mac'] = gl_gatewaymacaddr;
    return JSON.stringify(sendString);
}

function ChangeLightDevice(id)
{
    var targetUrl = gl_target_server + "/php/lightdevicecontrol.php";
    var p = $.getJSON(targetUrl, `loadlightsettingbyid&project_id=${project_id}&id=${id}`, function (data)
    {
        // $.each(data, (index, ev) => {
        //     gl_light_setting[id] = ev;
        // })
    })
    .success((data) => {
        if (data.length > 0)
            displayLightSetting(data[0]);
    })
    .error(err => {
        console.log(`lightdevicecontrol -> ChangeLightDevice : ${err}`);
    })
    .complete(() => {

    });
}

function displayLightSetting(data)
{
    $('#ui_power_switch').attr('data-onOff', data['state_onOff']);
    switch (data['state_onOff']) {
        case 0:
            $('#ui_power_switch').css('background-image', 'url(/images/power-off.png)')
            break;
        case 1:
            $('#ui_power_switch').css('background-image', 'url(/images/power-on.png)')
            break;
    }
    $('#ui_brightness').val(data['state_level']);
    $('#ui_colorTemperature').val(data['state_cct']);
}

$(document).ready(function() {

    $('#ls_list_content').on('click', '.listItem', (ev) => {
        $('.listItem').removeClass('listItemSelected');
        var t = ev.currentTarget.dataset['id'];
        $(`.listItem[data-id=${t}]`).addClass("listItemSelected");
        var key = ev.currentTarget.dataset['key'];
        ChangeLightDevice(key);
        // gl_brightMove = false;
        gl_situationTarget = '';
    });

    $('#light_controls').on('click', '#ui_brightness', (ev) => {
        console.log(`${ev.currentTarget.id}->${ev.currentTarget.value}`);
    })

    $('#ui_power_switch, #ui_brightness, #ui_colorTemperature').click(function(e) {
        //var payload = SendLightDeviceChange("onoff");
        var act = '';
        switch (e.currentTarget.id) {
            case "ui_power_switch":
                act = 'onOff';
                break;
            case "ui_brightness":
                act = 'level';
                break;
            case "ui_colorTemperature":
                act = 'cct';
                break;
            default:

        }
        SendCommand(act);
    });

    $('.situation').click(e => {
        var blnClose = $(`#${e.currentTarget.id}`).hasClass('situationSelected');
        $('.situation').removeClass('situationSelected');
        if (blnClose)
        {
            gl_situationTarget = '';
        }
        else
        {
            gl_situationTarget = e.currentTarget.id;
            $(`#${e.currentTarget.id}`).addClass('situationSelected');

            var cmd = '';
            var v = 0;
            var id = e.currentTarget.id;
            switch (id) {
                case 'div_bright':
                    v = parseInt($('#ui_brightness').val());
                    break;
                case 'div_color':
                    v = parseInt($('#ui_colorTemperature').val());
                    break;
            }
            if (id != 'div_breathe' && v > gl_moveval)
            {
                gl_range = -20;
                gl_mode = 'down';
            }

            switch (id) {
                case 'div_bright':
                    setBrightness();
                    break;
                case 'div_color':
                    setColor();
                    break;
                case 'div_breathe':
                    setbreathe();
                    break;
            }
        }

    });

    $('.moveswitch').click(e => {

        if (e.currentTarget.checked)
        {
            if (gl_situationTarget == '')
                gl_situationTarget = e.currentTarget.id;
            else if (gl_situationTarget != e.currentTarget.id)
            {
                document.getElementById(gl_situationTarget).checked = false;
                gl_situationTarget = e.currentTarget.id;
            }

            // gl_brightMove = true;
            var cmd = '';
            var v = 0;
            var id = e.currentTarget.id;
            switch (id) {
                case 'ui_chk_brightness':
                    v = parseInt($('#ui_brightness').val());
                    break;
                case 'ui_chk_color':
                    v = parseInt($('#ui_colorTemperature').val());
                    break;
            }
            if (id != 'ui_chk_breathe' && v > gl_moveval)
            {
                gl_range = -20;
                gl_mode = 'down';
            }

            switch (id) {
                case 'ui_chk_brightness':
                    setBrightness();
                    break;
                case 'ui_chk_color':
                    setColor();
                    break;
                case 'ui_chk_breathe':
                    setbreathe();
                    break;
            }

        }
        else
        {
            // gl_brightMove = false;
            gl_situationTarget = '';
        }

    })

    $('.targetTime').click(e => {
        $('.situation').removeClass('situationSelected');
        var act = e.currentTarget.dataset['key'];
        SendCommandTime(act);
    });

});

function SendCommand(type){
    // console.log(payload);

    var gateway_mac = gl_gatewaymacaddr;
    var cmd = '';

    switch (type) {
        case "onOff":
            if ($('#ui_power_switch').attr('data-onOff') == '1')
                $('#ui_power_switch').attr('data-onOff', '0').css('background-image', 'url(/images/power-off.png)');
            else
                $('#ui_power_switch').attr('data-onOff', '1').css('background-image', 'url(/images/power-on.png)');

            cmd = `${type}=${$('#ui_power_switch').attr('data-onOff')}`
            break;
        case "level":
            cmd = `${type}=${$('#ui_brightness').val()}`
            break;
        case "cct":
            cmd = `${type}=${$('#ui_colorTemperature').val()}`
            break;
    }

    var id = $('.listItemSelected').data('id');
    var uniAddress = $('.listItemSelected').data('uniaddress');
    var targetUrl = gl_target_server + "/php/lightdevicecontrol.php";
    var p = $.getJSON(targetUrl, `SendMqttCmd&project_id=${project_id}&id=${id}&gateway_mac=${gateway_mac}&cmd=${cmd}&uniAddress=${uniAddress}`, function (data) {
    })
    .success(data => {})
    .error(err => {
        console.log(err);
    })
    .complete(() => {})

}

function SendCommandTime(time)
{
    var gateway_mac = gl_gatewaymacaddr;
    var id = $('.listItemSelected').data('id');
    var uniAddress = $('.listItemSelected').data('uniaddress');
    var targetUrl = gl_target_server + "/php/lightdevicecontrol.php";
    var cmd = '';
    var level = 0;
    var cct = 0;

    gl_situationTarget = '';

    for (var i = 0; i < 2; i++) {
        switch (i) {
            case 0:
                // 亮度
                switch (time) {
                    case 'morning':
                        level = '250';
                        break;
                    case 'noon':
                        level = '125';
                        break;
                    case 'evening':
                        level = '50';
                        break;
                }
                cmd = `level=${level}`;
                $('#ui_brightness').val(level);
                break;
            case 1:
                // 色溫
                switch (time) {
                    case 'morning':
                        cct = '250';
                        break;
                    case 'noon':
                        cct = '125';
                        break;
                    case 'evening':
                        cct = '50';
                        break;
                }
                cmd = `cct=${cct}`;
                $('#ui_colorTemperature').val(cct);
                break;
        }

        var p = $.getJSON(targetUrl, `SendMqttCmd&project_id=${project_id}&id=${id}&gateway_mac=${gateway_mac}&cmd=${cmd}&uniAddress=${uniAddress}`, function (data) {
        })
        .success(data => {})
        .error(err => {
            console.log(err);
        })
        .complete(() => {})

    }
}

function setBrightness()
{

    var level = parseInt($('#ui_brightness').val());

    console.log(`level->${level}`);
    level += gl_range;
    if (gl_mode == 'up' && level > 255)
    {
        level = 255;
    }
    else if (gl_mode == 'down' && level < 0)
        level = 0;

    $('#ui_brightness').val(level);

    SendCommand('level');

    if (gl_mode == 'up' && level == 255)
    {
        gl_mode = 'down';
        gl_range = -20;
    }
    else if (gl_mode == 'down' && level == 0)
    {
        gl_mode = 'up';
        gl_range = 20;
    }

    if (gl_situationTarget == 'ui_chk_brightness' || gl_situationTarget == 'div_bright')
        setTimeout(setBrightness, 2000);

}

function setColor()
{

    var cct = parseInt($('#ui_colorTemperature').val());

    console.log(`cct->${cct}`);
    cct += gl_range;
    if (gl_mode == 'up' && cct > 255)
    {
        cct = 255;
    }
    else if (gl_mode == 'down' && cct < 0)
        cct = 0;

    $('#ui_colorTemperature').val(cct);

    SendCommand('cct');

    if (gl_mode == 'up' && cct == 255)
    {
        gl_mode = 'down';
        gl_range = -20;
    }
    else if (gl_mode == 'down' && cct == 0)
    {
        gl_mode = 'up';
        gl_range = 20;
    }

    if (gl_situationTarget == 'ui_chk_color' || gl_situationTarget == 'div_color')
        setTimeout(setColor, 2000);

}

function setbreathe()
{

    var onOff = $('#ui_power_switch').attr('data-onOff');
    switch (onOff) {
        case 't':
            onOff = '0';
            $('#ui_power_switch').attr('data-onOff', 'f');
            break;
        case 'f':
            onOff = '1';
            $('#ui_power_switch').attr('data-onOff', 't');
            break;
    }

    console.log(`onOff->${onOff}`);

    SendCommand('onOff');

    if (gl_situationTarget == 'ui_chk_breathe' || gl_situationTarget == 'div_breathe')
        setTimeout(setbreathe, 3000);

}

//開啟事件彙總表頁面
function open_lightdevicecontrol_page()
{
    var win = window.open('/index.php/vilslightdevicecontrol/'+project_id, '_blank');
    win.focus();
}
