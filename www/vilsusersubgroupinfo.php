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
    <title>SMIMS VILS IoT - User Sub Group</title>
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
            <h3 class="box-title"><?php echo $vilsusersubgroupinfo_title;?></h3>

            <span>&nbsp;</span>
            <?php
            //if ($GL_CONFIG_ENABLE_CREATE_NEW_SUBGROUP == 1 && ($_SESSION['type'] == 2 || $_SESSION["groupid"] == 1 || $_SESSION["groupid"] == 2))
            if ($GL_CONFIG_ENABLE_CREATE_NEW_SUBGROUP == 1 && ($_SESSION["groupid"] == 1 || $_SESSION["groupid"] == 2 || $_SESSION["groupid"] == 6))
            {
                echo "<a class='hidden btn btn-success btn-xs' onclick='create_new_usersubgroup();' id='btnCreateNewSubGroup'>".$vilsusersubgroupinfo_creatnewusergroup."</a>";
            }
            ?>

            <div class="box-tools pull-right">
                <ul class="pagination pagination-sm no-margin">
                  <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                  <li><a href="#" id="id_current_page_num">1</a></li>
                  <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
                </ul>
                <ul class="pagination pagination-sm no-margin pull-right">
                  <li>
                      <button type="button" class="btn btn-box-tool" onclick="open_usersubgroupinfo_page();return false;"><i class="fa fa-external-link"></i>
                      </button>
                  </li>
                </ul>
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body no-padding">
            <table class="table table-bordered">
                <thead>
                <tr style="background-color:#cce6ff;">
                  <th style="width: 10px">#</th>
                  <th><?php echo $vilsusersubgroupinfo_groupname;?></th>
                  <th><?php echo $vilsusersubgroupinfo_permission;?></th>
                  <th><?php echo $vilsusersubgroupinfo_photo;?></th>
                  <th><?php echo $vilsusersubgroupinfo_manager;?></th>
                  <?php
                  if ($GL_CONFIG_EDIT_USERINFO == 1)
                  {
                      echo "<th>"; echo $vilsusersubgroupinfo_operation; echo "</th>";
                  }
                  ?>
                </tr>
                </thead>
                <tbody id="usersubgroupinfo_TABLE">
                </tbody>
            </table>
            <div class="btn-group">
                <?php
                if ($GL_CONFIG_EDIT_USERINFO == 1)
                {
                    // echo "<button class='btn btn-block btn-warning btn-lg' id='createnewusersubgroupbutton' ><i class='fa fa-plus'></i>"; echo $vilsusersubgroupinfo_creatnewusergroup; echo"</button>";
                }
                ?>

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
    <script src="/js/usersubgroupinfo.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/usersubgroupinfo_src/00-usersubgroupinfo.js"></script>
    <script src="/js/storageimage_src/02-uploadimage.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
