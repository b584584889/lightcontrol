<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
    <link rel="shortcut icon" sizes="16x16 32x32 48x48 64x64" href="/images/favicon.ico">
    <link href="/css/vils.min.css" rel="stylesheet">
    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - Assist</title>
  </head>

      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $vilsassist_title;?></h3>

          <span>&nbsp;</span>
          <?php
            $groupid = $_SESSION["groupid"];
            if ($groupid == 1 || $groupid == 2)
            {
                echo '<a class="btn btn-success btn-xs" onclick="create_new_assist();">';echo $vilsassist_createnew; echo '</a>';
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
                  <button type="button" class="btn btn-box-tool" onclick="open_assist_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                </li>
              </ul>
          </div>
        </div>
        <!-- /.box-header -->
        <div class="box-body">
          <table class="table table-bordered" id="ASSIST_TABLE">
            <thead>
              <tr style="background-color:rgb(236, 240, 245);">
                <th style="width: 30px">#</th>
                <th>事項名稱</th>
                <th>事項說明</th>
                <th style="width: 300px">子集合權限</th>
                <th>負責人員</th>
                <?php
                if ($groupid == 1 || $groupid == 2)
                {
                    echo '<th>編輯</th>';
                }
                ?>
              </tr>
            </thead>
            <tbody id="ASSIST_TABLE_BODY">

            </tbody>
          </table>
        </div>
        <!-- /.box-body -->

        <div class="box-footer">
            <h4 class="box-title">協助事項設定</h4>

                <table class="table table-bordered">
                  <thead>
                    <tr style="background-color:rgb(216, 210, 215);">
                      <th>發出推播給負責人, 提前通知時間(單位:分鐘)</th>
                      <?php
                      if ($groupid == 1 || $groupid == 2)
                      {
                          echo '<th>編輯</th>';
                      }
                      ?>
                    </tr>
                  </thead>
                  <tbody>
                     <tr>
                         <td id="config_sendNotifyTime"></td>
                         <?php
                         if ($groupid == 1 || $groupid == 2)
                         {
                             echo '<td><button id="editconfigbtn" titile="Edit This Assist Config" href="javascript:;" onclick="edit_assist_config();"><i id="editconfigimg" class="fa fa-edit"></i></button></td>';
                         }
                         ?>
                     </tr>
                  </tbody>
                </table>


        </div>
        <!-- /.box-footer -->

      </div>
      <!-- /.box -->


    <script>
        var project_id = <?php echo $project_id ?>;
        var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
        var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
        var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/assist.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/assist_src/00-assist.js?date=<?php echo $UPDATE_DATE;?>"></script> -->
</html>
