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
    <!-- <link href="/css/vils_src/14-breadcrumb.css" rel="stylesheet"> -->
    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - Fence Alert</title>
  </head>



        <div class="row" id="fencealert_group_edit" style="display:block;">
          <div class="col-md-11">
            <!-- Event Settings -->
            <div class="box" id="fencealert_group_edit_box">
              <div class="box-header with-border">
                <h3 class="box-title"><?php echo $vilsfencealert_title;?></h3>
                <div class="box-tools pull-right">
                    <button type="button" class="btn btn-box-tool" onclick="open_fencealert_page();return false;"><i class="fa fa-external-link"></i>
                    </button>
                </div>
              </div>
              <!-- /.box-header -->
              <div class="box-body" id="fencealert_box_body">


              </div>
              <!-- /.box-body -->

              <div class="box-footer">
                  <!-- Split button -->
                  <div class="btn-group"><?php echo $vilsfencealert_Anchor;?></div>
                  <div class="btn-group">
                      <div class="btn-group" id="fencealert_anchor_selection"></div>
                      <div class="btn-group" id="fencealert_area_selection_text">&nbsp;&nbsp;<?php echo $vilsfencealert_Anchor_1;?><?php echo $vilsfencealert_At;?>&nbsp;&nbsp;</div>
                      <div class="btn-group" id="fencealert_area_selection"></div>
                      <div class="btn-group" id="fencealert_range_selection_text">&nbsp;&nbsp;<?php echo $vilsfencealert_Range;?>&nbsp;&nbsp;</div>
                      <div class="btn-group">
                          <input type="text" style="width:100px" id="fencealert_range_selection_input" value="300">
                      </div>
                  </div>

                  <div class="btn-group pull-right">
                    <button type="button" onclick="fencealert_group_create();return false;" class="btn btn-default pull-right"><i class="fa fa-plus"></i> <?php echo $vilsfencealert_create;?></button>
                  </div>
              </div>
              <!-- /.box-footer-->
            </div>
            <!-- /.box-->
          </div>
          <!-- /.col -->
        </div>
        <!-- /.row -->

    <script>
        var project_id = <?php echo $project_id ?>;
        var accountid = "<?php echo $_SESSION["accountid"] ?>";
        var accountname = "<?php echo $_SESSION["accountname"] ?>";
        var groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/fencealert.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/fencealert_src/00-fencealert.js"></script> -->

</html>
