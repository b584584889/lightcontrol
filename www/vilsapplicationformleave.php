<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
    <link href="/css/vils.min.css" rel="stylesheet">
    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - Leave Application Form</title>
  </head>

    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $left_side_bar_applicationform_leave;?></h3>

            <span>&nbsp;</span>
            <a class="btn btn-success btn-xs" onclick="create_new_applicationform_leave();" id="CREATE_NEW_LEAVEFORM"><?php echo $applicationform_leave_createnew;?></a>

            <span>&nbsp;</span>
            <span id="FILTER_NONE_SIGNOFF">
            <input type="checkbox" onclick="checkbox_filter_click();" id="FILTER_NONE_SIGNOFF_CHECKBOX"><?php echo $applicationform_leave_filter_nonesignoff;?></input>
            </span>

            <span>&nbsp;&nbsp;</span>
            <div class="btn-group">
              <button type="button" class="btn btn-default" id="current_targetdatetime" value=0></button>
              <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
                <span class="caret"></span>
                <span class="sr-only">Toggle Dropdown</span>
              </button>
              <ul class="dropdown-menu" id="targetdatetime_dropdown_menu" role="menu">
              </ul>
            </div>

            <div class="btn-group" id="targetdatetime_range_selector">
                <div class="input-group date">
                    <label>開始日期:</label>
                    <input type="text" readonly="readonly" style="width: 120px;" id="targetdatetime_startdate">
                    </input>
                    <label>結束日期:</label>
                    <input type="text" readonly="readonly" style="width: 120px;" id="targetdatetime_enddate">
                    </input>
                    <button type="button" class="btn btn-default" onclick="targetdatetime_range_click();">讀取</button>
                </div>
            </div>

            <span id="rollcalldevice_dropdown_selector">
                <span>&nbsp;&nbsp;</span>
                <div class="btn-group">
                  <button type="button" class="btn btn-default">NFC感應器</button>
                  <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
                    <span class="caret"></span>
                    <span class="sr-only">Toggle Dropdown</span>
                  </button>
                  <ul class="dropdown-menu" id="nfcdevice_dropdown_menu" role="menu">
                  </ul>
                </div>

                <span>&nbsp;&nbsp;</span>
                <div class="btn-group">
                  <button type="button" class="btn btn-default">UHF感應器</button>
                  <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
                    <span class="caret"></span>
                    <span class="sr-only">Toggle Dropdown</span>
                  </button>
                  <ul class="dropdown-menu" id="rfiddevice_dropdown_menu" role="menu">
                  </ul>
                </div>
                <button type="button" class="btn btn-default" onclick="clear_readerrecord_click();">清除讀取紀錄</button>
                <label id="cardscan_result_label"></label>
            </span>

            <div class="box-tools pull-right">
                <ul class="pagination pagination-sm no-margin">
                  <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                  <li><a href="#" id="id_current_page_num">1</a></li>
                  <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
                </ul>
                <ul class="pagination pagination-sm no-margin pull-right">
                  <li>
                    <button type="button" class="btn btn-box-tool" onclick="open_applicationformleave_page();return false;"><i class="fa fa-external-link"></i>
                    </button>
                  </li>
                </ul>
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body table-responsive">
            <table class="table table-bordered" id="APPFORM_TABLE">
                <thead>
                    <tr>
                        <th style="width: 100px;" class="bg-gray disabled"><?php echo $applicationform_leave_serialnum;?></th>
                        <th class="bg-gray disabled"><?php echo $applicationform_leave_applytime;?></th>
                        <th class="bg-gray disabled"><?php echo $applicationform_leave_user;?></th>
                        <th class="bg-gray disabled"><?php echo $applicationform_leave_type;?></th>
                        <th class="bg-gray disabled"><?php echo $applicationform_leave_reason;?></th>
                        <th class="bg-gray disabled"><?php echo $applicationform_leave_timerange;?></th>
                        <th class="bg-gray disabled"><?php echo $applicationform_leave_reissue;?></th>
                        <th class="bg-gray disabled"><?php echo $applicationform_leave_photo;?></th>
                        <th class="bg-gray disabled"><?php echo $applicationform_leave_file;?></th>
                        <th class="bg-gray disabled"><?php echo $applicationform_leave_applyusername;?></th>
                        <th class="bg-gray disabled"><?php echo $applicationform_leave_applyuserrelation;?></th>
                        <th style="width: 50px;" class="bg-gray disabled"><?php echo $applicationform_leave_status;?></th>
                        <th style="width: 50px;" class="bg-gray disabled"><?php echo $applicationform_leave_signoff;?></th>
                    </tr>
                </thead>
                <tbody id="APPFORM_TABLE_BODY">
                </tbody>
            </table>
        </div>
        <!-- /.box-body -->
    </div>
    <!-- /.box -->

    <script>
      var project_id = <?php echo $project_id ?>;
      var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
      var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_userid = "<?php echo $_SESSION["userid"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/applicationformleave.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/courses_src/03-bootstrap-timepicker.js"></script>
    <script src="/js/courses_src/04-bootstrap-datepicker.js"></script>
    <script src="/js/storagefile_src/02-uploadfile.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/storageimage_src/02-uploadimage.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/applicationformleave_src/00-applicationformleave.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
