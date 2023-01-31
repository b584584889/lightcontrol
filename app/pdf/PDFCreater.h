/*
 *  PDFCreater.h
 *
 *  Created on: 2019/08/21
 *      Author: ikki
 */
#ifndef __PDFCreater_h__
#define __PDFCreater_h__

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>

// for PDF
#include <math.h>
#include <setjmp.h>
#include "hpdf.h"

#include "../common.h"

typedef enum {
    PDF_IMAGE_PNG = 0,
    PDF_IMAGE_JPG
}PDF_IMGAE_TYPE;

class PDFCreater
{
public:
    PDFCreater();
    ~PDFCreater();

    void test_pdf_creater();
    void create_event_report(std::string pdf_filename, std::string report_date, std::string report_start_date, std::string report_end_date, UserStruct * user, std::list<EmployeeEventReportStruct*> * eventlist);
    void create_inout_event_report(std::string pdf_filename, std::string report_date, std::string report_start_date, std::string report_end_date, NodeStruct * node, std::list<AreaInOutEventStruct*> * nodeAreaInOutList);
    void create_inout_event_report(std::string pdf_filename, std::string report_date, std::string report_start_date, std::string report_end_date, NodeStruct * node, std::list<Pair_AreaInOutEventStruct*> * pair_inout_events);
    void create_firedevent_report(std::string pdf_filename, std::string report_date, std::string report_start_date, std::string report_end_date, NodeStruct * node, std::list<FiredEventStruct*> * nodeFiredEventsList);
    void create_inspection_report(std::string pdf_filename, std::string report_date, std::string report_start_date, std::string report_end_date, PDFTableDataStruct * pdfData);

    void create_report_header(std::string pdf_filename, std::string report_title, std::string report_date, std::string report_start_date, std::string report_end_date);
    void create_report_userinfo(std::string report_type, std::string report_user);
    void create_report_tableinfo(std::string info, int font_size);
    void create_report_table(PDFTableDataStruct * pdfData);
    void create_report_table_old(PDFTableDataStruct * pdfData);
    void generate_report();

private:

    std::string m_pdf_filename;
    HPDF_Doc m_pdf;
    HPDF_Page m_page;
    HPDF_Font m_font_en_num;
    HPDF_Font m_font_big5_bold;
    HPDF_Font m_font_big5_light;
    HPDF_Font m_font_big5_regular;
    HPDF_Destination m_dst;
    int m_yoffset;

    void draw_line(HPDF_Page page, int startX, int startY, int endX, int endY, float lineWidth);
    void draw_rect(HPDF_Page page, int startX, int startY, int width, int height, float lineWidth);
    int draw_big5_text(HPDF_Page page, int posX, int posY, std::string in_string, HPDF_Font font, int font_size);
    int draw_big5_rect_text(HPDF_Page page, int posX, int posY, int width, std::string in_string, HPDF_Font font, int font_size);
    int draw_events(HPDF_Page page, int startX, int startY, std::string event_time, std::string resident, std::string event_process_time, std::string event_comment, HPDF_Font font, int font_size);
    int draw_inout_events(HPDF_Page page, int startX, int startY, std::string in_datetime, std::string out_datetime, std::string area_name, std::string total_time, HPDF_Font font, int font_size);
    int draw_firedevent_events(HPDF_Page page, int startX, int startY, std::string event_datetime, std::string user, std::string action, std::string area_name, std::string in_datetime, std::string out_datetime, std::string total_time, HPDF_Font font, int font_size);
    void draw_image(HPDF_Doc pdf, PDF_IMGAE_TYPE img_type, const char * filename, float x, float y, float draw_width);
    void show_description(HPDF_Page page, float x, float y, const char * text);
    //const char * utf_encode(const char *code, const char *in_buf, size_t in_len);
    std::string utf_encode(const char *code, std::string in_string);
    std::string getUserType(int type);
    std::string getLocatorType(int type);
};

#endif
