<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
    include ('vi_html_head.php');
    ?>


    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $userrollcallrecord_title;?></h3>
            <div class="box-tools pull-right">
                <ul class="pagination pagination-sm no-margin">
                  <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                  <li><a href="#" id="id_current_page_num">1</a></li>
                  <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
                </ul>
                <ul class="pagination pagination-sm no-margin pull-right">
                  <li>
                    <button type="button" class="btn btn-box-tool" onclick="open_userrollcallrecord_page();return false;"><i class="fa fa-external-link"></i>
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
                  <th><?php echo $userrollcallrecord_name;?></th>
                  <th><?php echo $userrollcallrecord_user;?></th>
                  <th><?php echo $userrollcallrecord_cardid;?></th>
                  <th><?php echo $userrollcallrecord_device;?></th>
                  <th><?php echo $userrollcallrecord_present;?></th>
                  <th><?php echo $userrollcallrecord_time;?></th>
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
      var project_id = 1;
      var accountid = "<?php echo $_SESSION["accountid"] ?>";
      var accountname = "<?php echo $_SESSION["accountname"] ?>";
      var groupid = "<?php echo $_SESSION["groupid"] ?>";

    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/userrollcallrecord.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/userrollcallrecord_src/00-userrollcallrecord.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
