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
    <title>SMIMS VILS IoT - User RollCall Result</title>
  </head>

  <div class="modal fade" id="modal-loading">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <h4 class="modal-title">資料載入中</h4>
        </div>
        <div class="modal-body">
          <div>讀取點名紀錄...</div>
        </div>
      </div>
      <!-- /.modal-content -->
    </div>
    <!-- /.modal-dialog -->
  </div>
  <!-- /.modal -->


    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $userrollcallresult_title;?></h3>

            <span>&nbsp;</span>
            <a class="btn btn-success btn-xs" onclick="create_userrollcallresult_xlsx_report();" id="xlsxreport_btn">產生XLSX檔案</a>
            <span id="xlsxreport_status"></span>

            <div class="box-tools pull-right">
              </ul>
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body no-padding table-responsive">

            <table class="table table-bordered">
                <thead>
                <tr style="background-color:#cce6ff;">
                  <th style="width: 10px">#</th>
                  <th><?php echo $userrollcallresult_name;?></th>
                  <th><?php echo $userrollcallresult_startdatetime;?></th>
                  <th><?php echo $userrollcallresult_enddatetime;?></th>
                  <th><?php echo $userrollcallresult_totalcount;?></th>
                  <th><?php echo $userrollcallresult_presentcount;?></th>
                  <th><?php echo $userrollcallresult_absentcount;?></th>
                </tr>
                </thead>
                <tbody id="USER_ROLLCALLRESULT_TABLE">
                </tbody>
            </table>
        </div>
        <!-- /.box-body -->
    </div>
    <!-- /.box -->

    <div class="box">
        <div class="box-body no-padding">
            <!-- Custom Tabs (Pulled to the right) -->
            <div class="nav-tabs-custom">
               <ul class="nav nav-tabs" id="USER_ROLLCALLRECORD_TABLE_TABS">
                   <!-- tags  -->
               </ul>
               <div class="tab-content" id="USER_ROLLCALLRECORD_TABLE_CONTENT">
                   <!-- tab-pane -->
               </div>
               <!-- /.tab-content -->
            </div>
            <!-- /.nav-tabs-custom -->
        </div>
        <!-- /.box-body -->
    </div>
    <!-- /.box -->


    <script>
      var project_id = <?php echo $project_id ?>;
      var accountid = "<?php echo $_SESSION["accountid"] ?>";
      var accountname = "<?php echo $_SESSION["accountname"] ?>";
      var groupid = "<?php echo $_SESSION["groupid"] ?>";

      var rollcallid = "<?php echo $rollcallid ?>";
      var eventid = "<?php echo $eventid ?>";

    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/userrollcallresult.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/userrollcallresult_src/00-userrollcallresult.js"></script> -->

</html>
