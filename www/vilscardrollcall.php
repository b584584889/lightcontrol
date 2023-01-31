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

    <style type="text/css">
    .datepicker table tr td.active:active,
    .datepicker table tr td.active:hover:active,
    .datepicker table tr td.active.disabled:active,
    .datepicker table tr td.active.disabled:hover:active,
    .datepicker table tr td.active.active,
    .datepicker table tr td.active:hover.active,
    .datepicker table tr td.active.disabled.active,
    .datepicker table tr td.active.disabled:hover.active
    {
      background-image: linear-gradient(to bottom, #cc4400, #994411);
    }
    /* .datepicker table tr td.highlight:active,
    .datepicker table tr td.highlight:hover:active,
    .datepicker table tr td.highlight.disabled:active,
    .datepicker table tr td.highlight.disabled:hover:active,
    .datepicker table tr td.highlight.active,
    .datepicker table tr td.highlight:hover.active,
    .datepicker table tr td.highlight.disabled.active,
    .datepicker table tr td.highlight.disabled:hover.active
    {
        background-image: linear-gradient(to bottom, #cc4400, #994411);
    } */
    .datepicker table tr td.new { display: none; }
    .datepicker table tr td.old { visibility: hidden; }
    .datepicker {
        background-color: #9fc49f !important;
        color: #333333 !important;
    }

    </style>

    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - RFID Card RollCall</title>
  </head>

    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $cardrollcall_title;?></h3>

            <span>&nbsp;</span>
            <a class="btn btn-success btn-xs" onclick="create_new_cardrollcall();"><?php echo $cardrollcall_creatnewrollcall;?></a>

            <div class="box-tools pull-right">
                <ul class="pagination pagination-sm no-margin">
                  <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                  <li><a href="#" id="id_current_page_num">1</a></li>
                  <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
                </ul>
                <ul class="pagination pagination-sm no-margin pull-right">
                  <li>
                    <button type="button" class="btn btn-box-tool" onclick="open_cardrollcall_page();return false;"><i class="fa fa-external-link"></i>
                    </button>
                </li>
              </ul>
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body no-padding table-responsive">

            <table class="table table-bordered">
                <thead>
                <tr style="background-color:#cce6ff;">
                  <th style="width: 10px">#</th>
                  <th><?php echo $cardrollcall_name;?></th>
                  <th style="width: 200px"><?php echo $cardrollcall_timestart;?></th>
                  <th style="width: 200px"><?php echo $cardrollcall_timeend;?></th>
                  <th><?php echo $cardrollcall_date;?></th>
                  <th><?php echo $cardrollcall_device;?></th>
                  <th><?php echo $cardrollcall_subgroups;?></th>
                  <th><?php echo $cardrollcall_pushnotify;?></th>
                  <th><?php echo $cardrollcall_operation;?></th>
                </tr>
                </thead>
                <tbody id="RFID_ROLLCALL_TABLE">
                </tbody>
            </table>
            <div class="btn-group">
                <?php
                    // echo "<button class='btn btn-block btn-warning btn-lg' id='createnewcardrollcallbutton' ><i class='fa fa-plus'></i>"; echo $cardrollcall_creatnewrollcall; echo "</button>";
                ?>
            </div>
        </div>
        <!-- /.box-body -->
    </div>
    <!-- /.box -->

    <script>
      var project_id = <?php echo $project_id ?>;
      var accountid = "<?php echo $_SESSION["accountid"] ?>";
      var accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/cardrollcall.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/cardrollcall_src/00-bootstrap-timepicker.js"></script>
    <script src="/js/cardrollcall_src/01-bootstrap-datepicker.js"></script>
    <script src="/js/locales/bootstrap-datepicker.zh-TW.min.js"></script>
    <script src="/js/cardrollcall_src/02-cardrollcall.js"></script> -->

</html>
