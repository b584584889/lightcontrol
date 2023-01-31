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
    <title>SMIMS VILS IoT - Sections</title>
  </head>



        <div class="row" id="section_group_edit" style="display:block;">
          <div class="col-md-11">
            <!-- Event Settings -->
            <div class="box" id="section_group_edit_box">
              <div class="box-header with-border">
                <h3 class="box-title"><?php echo $vilssection_title;?></h3>
                <div class="box-tools pull-right">
                    <button type="button" class="btn btn-box-tool" onclick="open_sections_page();return false;"><i class="fa fa-external-link"></i>
                    </button>
                    <!-- <a href="javascript:void(0)" onclick="open_sections_page();return false;"><i class="fa fa-external-link"></i></a> -->
                </div>
              </div>
              <!-- /.box-header -->
              <div class="box-body" id="section_box_body">


              </div>
              <!-- /.box-body -->

              <div class="box-footer">
                  <!-- Split button -->
                  <div class="btn-group" id="section_type_selection">
                  </div>
                  <div class="btn-group"><?php echo $vilssection_Anchor;?></div>
                  <div class="btn-group" id="section_type_0D_btn-group">
                      <div class="btn-group" id="section_anchor_selection"></div>
                      <div class="btn-group" id="section_area_selection_text">&nbsp;&nbsp;<?php echo $vilssection_Anchor_1;?><?php echo $vilssection_At;?>&nbsp;&nbsp;</div>
                      <div class="btn-group" id="section_area_selection"></div>
                      <div class="btn-group" id="section_range_selection_text">&nbsp;&nbsp;<?php echo $vilssection_Range;?>&nbsp;&nbsp;</div>
                      <div class="btn-group">
                          <input type="text" style="width:100px" id="section_range_selection_input" value="300">
                      </div>
                      <div class="btn-group" id="section_alerttype_selection_text">&nbsp;&nbsp;Alert Type&nbsp;&nbsp;</div>
                      <div class="btn-group" id="section_alerttype_selection"></div>
                  </div>

                  <div class="btn-group" id="section_type_1D_btn-group">
                      <!-- <div class="btn-group" id="section_anchor_selection_1"></div>
                      <div class="btn-group" id="section_anchor_selection_text">&nbsp;&nbsp;And&nbsp;&nbsp;</div>
                      <div class="btn-group" id="section_anchor_selection_2"></div> -->
                      <div class="btn-group">
                          <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown" id="dropdown_cur_anchor">Anchors<span class="caret"></span></button>
                          <ul class="dropdown-menu" id="dropdown_anchors">
                          </ul>
                      </div>
                      <div class="btn-group" id="section_frompoint_selection_text">&nbsp;&nbsp;<?php echo $vilssection_Anchor_1;?><?php echo $vilssection_start_at;?>&nbsp;&nbsp;</div>
                      <div class="btn-group">
                          <input type="text" style="width:100px" id="section_frompoint_selection_input" value="0,0,0">
                      </div>
                      <div class="btn-group" id="section_topoint_selection_text">&nbsp;&nbsp;<?php echo $vilssection_to_end;?>&nbsp;&nbsp;</div>
                      <div class="btn-group">
                          <input type="text" style="width:100px" id="section_topoint_selection_input" value="0,0,0">
                      </div>
                  </div>


                  <div class="btn-group pull-right">
                    <button type="button" onclick="section_group_create();return false;" class="btn btn-default pull-right"><i class="fa fa-plus"></i> <?php echo $vilssection_create;?></button>
                  </div>
              </div>
              <!-- /.box-footer-->
            </div>
            <!-- /.box-->
          </div>
          <!-- /.col -->
        </div>
        <!-- /.row -->

        <!-- <div class="breadcrumb section0">
        	<a class="active">0D</a>
        	<a href="#">Compare</a>
        	<a href="#">Order Confirmation</a>
        	<a href="#">Checkout</a>
        </div> -->

    <script>
        var project_id = <?php echo $project_id ?>;
        var accountid = "<?php echo $_SESSION["accountid"] ?>";
        var accountname = "<?php echo $_SESSION["accountname"] ?>";
        var groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <!-- <script src="/js/sections.min.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

    <script src="/js/sections_src/00-section_group.js"></script>

</html>
