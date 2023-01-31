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
    <title>SMIMS VILS IoT - User</title>
  </head>


          <div class="modal fade" id="modal-qrcode">
            <div class="modal-dialog">
              <div class="modal-content">
                <div class="modal-header">
                  <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                    <span aria-hidden="true">&times;</span></button>
                  <h4 class="modal-title">QR Code</h4>
                </div>
                <div class="modal-body">
                  <div id='device_qrcode_type'></div>
                  <div id='device_qrcode_name'></div>
                  <div id='device_qrcode_macaddress'></div>
                  <div id='device_qrcode_target'></div>
                  <div id='device_qrcode_topic'></div>
                  <div id='device_qrcode_datetime'></div>
                  <img src='' alt='' class='margin'width='250' id='device_qrcode_ui'>
                </div>
                <div class="modal-footer">
                  <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
                </div>
              </div>
              <!-- /.modal-content -->
            </div>
            <!-- /.modal-dialog -->
          </div>
          <!-- /.modal -->


    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $vilsuserinfo_title;?></h3>

            <span>&nbsp;</span>
            <?php
            if ($GL_CONFIG_EDIT_USERINFO == 1)
            {
                echo "<a class='btn btn-danger btn-xs' onclick='create_new_user();'>".$vilsuserinfo_creatnewuser."</a>";
            }
            ?>

            <span>&nbsp;</span>
            <a class="btn btn-info btn-xs" onclick="read_xlsx_input();" id="xlsxinput_btn">載入XLSX檔案</a>
            <input id="file-input" type="file" name="name" style="display: none;" onchange="inputfile_onchange();"/>

            <span>&nbsp;</span>
            <a class="btn btn-success btn-xs" onclick="create_user_xlsx_report();" id="xlsxreport_btn">產生XLSX檔案</a>
            <span id="xlsxreport_status"></span>

            <div class="box-tools pull-right">
                <ul class="pagination pagination-sm no-margin">
                  <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                  <li><a href="#" id="id_current_page_num">1</a></li>
                  <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
                </ul>
                <ul class="pagination pagination-sm no-margin pull-right">
                  <li>
                    <button type="button" class="btn btn-box-tool" onclick="open_userinfo_page();return false;"><i class="fa fa-external-link"></i>
                    </button>
                  </li>
                </ul>
            </div>
            <div class="btn-group">
                <input type="text" id="map_search_text" placeholder="尋找人員">
            </div>

            <span>&nbsp;&nbsp;</span>
            <div class="btn-group">
              <button type="button" class="btn btn-default" id="current_subgroup_title"></button>
              <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
                <span class="caret"></span>
                <span class="sr-only">Toggle Dropdown</span>
              </button>
              <ul class="dropdown-menu" id="dropdown_menu_subgroup" role="menu">
              </ul>
            </div>

        </div>
        <!-- /.box-header -->

        <div class="box-body no-padding table-responsive">

            <table class="table table-bordered">
                <thead>
                <tr style="background-color:#cce6ff;">
                  <th style="width: 10px">#</th>
                  <th style="width: 80px"><?php echo $vilsuserinfo_name;?></th>
                  <th><?php echo $vilsuserinfo_account;?></th>
                  <th><?php echo $vilsuserinfo_id;?></th>
                  <th><?php echo $vilsuserinfo_usertitle;?></th>
                  <th><?php echo $vilsuserinfo_photo;?></th>
                  <th><?php echo $vilsuserinfo_group;?></th>
                  <th><?php echo $vilsuserinfo_datagroup;?></th>
                  <th><?php echo $vilsuserinfo_subgroup;?></th>
                  <th><?php echo $vilsuserinfo_rfidcards;?></th>
                  <th><?php echo $vilsuserinfo_nfccards;?></th>
                  <th><?php echo $vilsuserinfo_uwbcards;?></th>
                  <th><?php echo $vilsuserinfo_sensors;?></th>
                  <?php
                  if ($GL_CONFIG_EDIT_USERINFO == 1)
                  {
                      echo "<th>"; echo $vilsuserinfo_operation; echo "</th>";
                  }
                  ?>
                </tr>
                </thead>
                <tbody id="USER_TABLE">
                </tbody>
            </table>
            <div class="btn-group">
            </div>
        </div>
        <!-- /.box-body -->
    </div>
    <!-- /.box -->

    <script>
      var project_id = <?php echo $project_id ?>;
      var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
      var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
      var config_enable_edit = <?php echo $GL_CONFIG_EDIT_USERINFO ?>;
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <!-- <script src="/js/userinfo.min.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

    <script src="/js/userinfo_src/00-userinfo.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/storageimage_src/02-uploadimage.js?date=<?php echo $UPDATE_DATE;?>"></script>

</html>
