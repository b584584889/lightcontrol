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
    <title>SMIMS VILS IoT - RFID Card RollCall Result</title>
  </head>

    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $cardrollcallresult_title;?></h3>
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
                  <th><?php echo $cardrollcallresult_name;?></th>
                  <th><?php echo $cardrollcallresult_startdatetime;?></th>
                  <th><?php echo $cardrollcallresult_enddatetime;?></th>
                  <th><?php echo $cardrollcallresult_totalcount;?></th>
                  <th><?php echo $cardrollcallresult_presentcount;?></th>
                  <th><?php echo $cardrollcallresult_absentcount;?></th>
                </tr>
                </thead>
                <tbody id="RFID_ROLLCALLRESULT_TABLE">
                </tbody>
            </table>
        </div>
        <!-- /.box-body -->
    </div>
    <!-- /.box -->

    <div class="box">
        <div class="box-body no-padding">
            <table class="table table-bordered">
                <thead>
                <tr style="background-color:#cce6ff;">
                  <th style="width: 10px">#</th>
                  <th><?php echo $cardrollcallresult_cardid;?></th>
                  <th><?php echo $cardrollcallresult_device;?></th>
                  <th><?php echo $cardrollcallresult_user;?></th>
                  <th><?php echo $cardrollcallresult_present;?></th>
                  <th><?php echo $cardrollcallresult_time;?></th>
                  <th><?php echo $cardrollcallresult_operation;?></th>
                </tr>
                </thead>
                <tbody id="RFID_ROLLCALLRECORD_TABLE">
                </tbody>
            </table>
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
    <script src="/js/cardrollcallresult.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/cardrollcallresult_src/00-cardrollcallresult.js"></script> -->

</html>
