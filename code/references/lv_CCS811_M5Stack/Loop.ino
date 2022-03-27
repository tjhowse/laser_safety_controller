//-------------------------------------------------------------------
uint32_t tabview_time = 5000;        // Auto Time
uint32_t lastMillis = 0; 
uint32_t last = 0;
uint16_t eco2, etvoc, errstat, raw;  // Read data
int first = 0;

void loop() {
  lv_task_handler(); delay(5);
  
  if ( millis()-lastMillis > period*1000 ){
    lastMillis = millis();
    ccs811.read(&eco2, &etvoc, &errstat, &raw);
    if ( errstat == CCS811_ERRSTAT_OK ){
      Serial.print("eCO2    ");Serial.println(eco2);
      Serial.print("eTVOC   ");Serial.println(etvoc);
      Serial.print("ERRSTAT ");Serial.println(errstat);
      Serial.print("RAW     ");Serial.println(raw);Serial.println();
      if ( eco2 > 300 ){
        if (first == 0){
          lv_obj_set_hidden(page,true);
          lv_win_clean(page);
          lv_label_set_text_fmt(label," ");
          first = 1;
        } 
        lv_label_set_text_fmt(txt2,   "%d", eco2 );
        lv_label_set_text_fmt(txt5,   "%d", etvoc);
        lv_gauge_set_value   (gauge,     0, eco2 );
        lv_chart_set_next    (chart1, ser1, eco2 ); 
        lv_chart_set_next    (chart2, ser2, etvoc); 
        if ( eco2 > 2000 && etvoc > 250 ){ ESP.restart();}
      }
    }  
  }
  if ( !auto_man ) {    // Auto or Manual tab
    if ( first==1 && eco2>300 && (millis()-last)>tabview_time){
      last = millis();  // Auto : Advance tab automatically
      tab_pos = tab_pos + 1; if(tab_pos>(tb-1)){tab_pos=0;}
      lv_tabview_set_tab_act(tabview, tab_pos, false); // next tab
    }
  }
}
//-------------------------------------------------------------------
