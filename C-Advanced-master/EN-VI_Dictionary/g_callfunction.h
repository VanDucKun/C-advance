#include <gtk/gtk.h>
#include <string.h>
#include "btree.h"
#include "soundex.h"

GtkWidget *textView, *window;
GtkWidget *textSearch;
BTA *data, *soundexTree;
GtkTextBuffer *buffer;
GtkWidget *about_us_dialog;
GtkListStore *suggestList;

void destroy_c(GtkWidget *widget, gpointer gp){
	gtk_widget_destroy((GtkWidget *)gp);
}
int prefix(char *s1, char *s2){
	int len1=strlen(s1);
	int len2=strlen(s2),i;
	if(len1>len2) return 0;
	else{
		for(i=0;i<len1;i++){
			if(s1[i]!=s2[i])
				return 0;
		}
	}
	return 1;
}

char suggestion[15][100];
void suggest_function(GtkWidget* widget, gpointer soundexTree){
	int rsize, i=0;
	char en[100],soundex[5], word[50], tmp[1000], meaning[10000];
	strcpy(tmp, "DO YOU MEAN??\n-");
	strcpy(word, gtk_entry_get_text(GTK_ENTRY(textSearch)) );
	if(gtk_entry_get_text_length(GTK_ENTRY(textSearch)) > 0){
			btsel(soundexTree,"", soundex,5*sizeof(char),&rsize);
			while(btseln(soundexTree, en, soundex, 5*sizeof(char),&rsize)==0 && i<15){
				if(prefix(word, en)==1){				
						strcpy(suggestion[i],en);
						strcat(tmp, suggestion[i]);
						strcat(tmp, "\n-");
						i++;				
				}
			}
			tmp[strlen(tmp)-1] = '\0';
			gtk_text_buffer_set_text(buffer, tmp, -1);		
	}
	else{
		gtk_text_buffer_set_text(buffer, "\n", -1);
	}
}


void auto_complete(GtkWidget* widget, gpointer soundexTree){
	gtk_entry_set_text(GTK_ENTRY(textSearch), suggestion[0]);
	//display_meaning(widget, soundexTree);
}

void make_about_us_dialog() {
	about_us_dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_us_dialog), "English-Vietnamese Dictionary");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_us_dialog), "C-Advance 20182");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_us_dialog), "DHBK HN");
	
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_us_dialog), "Nguyen Tai Thao\n"
	                              "Luong Duc Minh\n"
	                              "Vu Nguyen Khanh\n"
									"Nguyen thi Ngan\n\n");
	gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(about_us_dialog), NULL);

}


void show_about_us_dialog(GtkWidget *widget, gpointer point){
	make_about_us_dialog();
	gtk_dialog_run(GTK_DIALOG(about_us_dialog));
	gtk_widget_destroy(about_us_dialog);
}


void display_meaning(GtkWidget *widget, gpointer soundexTree){
	int rsize, beExisted, i;
	char word[50], meaning[5000], suggest[100][100];
	strcpy(word, gtk_entry_get_text(GTK_ENTRY(textSearch)));
	beExisted = btsel(data, word, meaning, 5000, &rsize);
	if(beExisted == 0){
		gtk_text_buffer_set_text(buffer, meaning, -1);
	}
	else{
				gtk_text_buffer_set_text(buffer, "Opps! We have no idea about this word", -1);	
	}
}

typedef struct{
		GtkWidget * data_array[3];
		BTA *tree;
	}data_struct;

void Show_message(GtkWidget * parent , GtkMessageType type,  char * mms, char * content) {
	GtkWidget *mdialog;
	mdialog = gtk_message_dialog_new(GTK_WINDOW(parent),
	                                 GTK_DIALOG_DESTROY_WITH_PARENT,
	                                 type,
	                                 GTK_BUTTONS_OK,
	                                 "%s", mms);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(mdialog), "%s",  content);
	gtk_dialog_run(GTK_DIALOG(mdialog));
	gtk_widget_destroy(mdialog);
}

void add_new_word(GtkWidget *widget, gpointer pointer){
	char word[50], meantext[10000], soundex[5];
	int rsize, insertDictResult, insertSoundexResult;
	data_struct *pt = (data_struct*)pointer;
	GtkWidget *inputtext = (GtkWidget**)(pt->data_array[0]);
	GtkWidget *mean = (GtkWidget**)(pt->data_array[1]);
	GtkWidget *dialog = (GtkWidget**)(pt->data_array[2]);
	BTA *soundexTree = (BTA**)(pt->tree);
	GtkTextIter st_iter;
	GtkTextIter ed_iter;
	gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(mean)), &st_iter);
	gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(mean)), &ed_iter);
	gtk_text_buffer_get_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(mean)), &st_iter, &ed_iter, FALSE);

	strcpy(meantext, gtk_text_buffer_get_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(mean)), &st_iter, &ed_iter, FALSE));
	strcpy(word, gtk_entry_get_text(GTK_ENTRY(inputtext)));

	if(word[0] == '\0' || meantext[0] == '\0')
		Show_message(dialog, GTK_MESSAGE_WARNING, "C???nh b??o!", "Kh??ng ???????c b??? tr???ng ph???n n??o.");
	else if(btsel(data, word, meantext, 5000, &rsize) == 0)
		Show_message(dialog, GTK_MESSAGE_WARNING, "X???y ra l???i!", "T??? v???a nh???p ???? c?? trong t??? ??i???n.");
	else{
		insertDictResult = btins(data, word, meantext, strlen(meantext) + 1);
		SoundEx(soundex, word, 4, 1);
    	insertSoundexResult = btins(soundexTree, word, soundex, 5*sizeof(char));
		if(insertDictResult == 0 && insertSoundexResult == 0)
			Show_message(dialog, GTK_MESSAGE_INFO, "Th??nh c??ng!", "???? th??m t??? v???a nh???p v??o t??? ??i???n.");
		else
			Show_message(dialog, GTK_MESSAGE_INFO, "X???y ra l???i!", "C?? l???i b???t ng??? x???y ra.");
	}
	
}
void show_add_dialog(GtkWidget *widget, gpointer soundexTree){
	GtkWidget *adddialog;
	adddialog = gtk_dialog_new_with_buttons("Th??m t???", GTK_WINDOW(window),
	                                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, NULL, NULL);

	GtkWidget *dialog_ground = gtk_fixed_new();

	GtkWidget* tframe = gtk_frame_new("T??? v???ng:");
	gtk_widget_set_size_request(tframe, 300, 50);
	GtkWidget* inputtext = gtk_entry_new();
	gtk_widget_set_margin_left(inputtext, 2);
	gtk_widget_set_margin_right(inputtext, 2);
	gtk_container_add(GTK_CONTAINER(tframe), inputtext);
	gtk_fixed_put(GTK_FIXED(dialog_ground), tframe, 0, 0);

	GtkWidget* bframe = gtk_frame_new("Ngh??a:");
	gtk_widget_set_size_request(bframe, 300, 200);
	GtkWidget* mean = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(mean), GTK_WRAP_WORD_CHAR);		//Chong tran be ngang
	GtkWidget* scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_fixed_put(GTK_FIXED(dialog_ground), bframe, 0, 55);
	gtk_container_add(GTK_CONTAINER(scroll), mean);
	gtk_container_add(GTK_CONTAINER(bframe), scroll);

	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	GtkWidget * OkButton =  gtk_button_new_with_label("Th??m");
	GtkWidget * CancelButton = gtk_button_new_with_label("H???y");
	gtk_box_pack_start(GTK_BOX(box), OkButton, TRUE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(box), CancelButton, TRUE, TRUE, 2);
	gtk_widget_set_size_request(box, 300, 50);
	gtk_widget_set_size_request(OkButton, 100, 40);
	gtk_widget_set_size_request(CancelButton, 100, 40);
	gtk_fixed_put(GTK_FIXED(dialog_ground), box, 0, 260);
	
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(adddialog))), dialog_ground,  TRUE, TRUE, 0);
	
	
	data_struct pointer;
	pointer.data_array[0] = inputtext;
	pointer.data_array[1] = mean;
	pointer.data_array[2] = adddialog;
	pointer.tree = soundexTree;
	g_signal_connect(OkButton, "clicked", G_CALLBACK(add_new_word), &pointer);
	g_signal_connect(CancelButton, "clicked", G_CALLBACK(destroy_c), adddialog);

	gtk_widget_show_all(adddialog);
	gtk_dialog_run(GTK_DIALOG(adddialog));
	gtk_widget_destroy(adddialog);
}

void edit_word(GtkWidget *widget, gpointer pointer){
	data_struct* pt = (data_struct*)pointer;
	GtkWidget *inputtext = (GtkWidget**)(pt->data_array[0]);
	GtkWidget *mean = (GtkWidget**)(pt->data_array[1]);
	GtkWidget *dialog = (GtkWidget**)(pt->data_array[2]);
	BTA *soundexTree = (BTA**)(pt->tree);
	GtkTextIter st_iter;
	GtkTextIter ed_iter;
	int result, rsize;
	char word[50], meantext[10000];
	gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(mean)), &st_iter);
	gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(mean)), &ed_iter);

	strcpy(word, gtk_entry_get_text(GTK_ENTRY(inputtext)));
	
	
	if (word[0] == '\0' || meantext[0] == '\0')
		Show_message(dialog, GTK_MESSAGE_WARNING, "C???nh b??o!",
		             "Kh??ng ???????c b??? tr???ng ph???n n??o.");
	else if(btsel(data, word, meantext, 5000, &rsize) != 0)
		Show_message(dialog, GTK_MESSAGE_ERROR, "X???y ra l???i!",
		             "Kh??ng t??m th???y t??? n??y trong t??? ??i???n.");
	else {
		strcpy(meantext, gtk_text_buffer_get_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(mean)), &st_iter, &ed_iter, FALSE));
		result = btupd(data, word, meantext, strlen(meantext) + 1);
		
		if (result == 0){
			Show_message(dialog, GTK_MESSAGE_INFO, "Th??nh c??ng!",
			             "???? c???p nh???t l???i ngh??a c???a t??? trong t??? ??i???n.");
		}
		else
			Show_message(dialog, GTK_MESSAGE_ERROR, "X???y ra l???i!", "C?? l???i b???t ng??? x???y ra.");
	}
}

void show_edit_dialog(GtkWidget *widget, gpointer soundexTree){
	int rsize;
	char word[50], meantext[10000];
	strcpy(word, gtk_entry_get_text(GTK_ENTRY(textSearch)));
	if (gtk_entry_get_text_length(GTK_ENTRY(textSearch)) == 0 ||
	        btsel(data, word, meantext, 5000, &rsize) != 0) {
		Show_message(window, GTK_MESSAGE_WARNING, "C???nh b??o:", "T??? v???a nh???p kh??ng c?? trong t??? ??i???n!");
		return;
	}
	GtkWidget *editdialog;
	editdialog = gtk_dialog_new_with_buttons("S???a t???", GTK_WINDOW(window),
	             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, NULL, NULL);

	GtkWidget *dialog_ground = gtk_fixed_new();
	
	GtkWidget* tframe = gtk_frame_new("T??? v???ng:");
	gtk_widget_set_size_request(tframe, 300, 50);
	GtkWidget* inputtext = gtk_search_entry_new();
	gtk_widget_set_margin_left(inputtext, 2);
	gtk_widget_set_margin_right(inputtext, 2);
	gtk_container_add(GTK_CONTAINER(tframe), inputtext);
	gtk_fixed_put(GTK_FIXED(dialog_ground), tframe, 0, 0);

	
	GtkWidget* bframe = gtk_frame_new("Ngh??a:");
	gtk_widget_set_size_request(bframe, 300, 200);
	GtkWidget* mean = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(mean), GTK_WRAP_WORD_CHAR);//Chong tran be ngang
	GtkWidget* scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scroll), mean);
	gtk_container_add(GTK_CONTAINER(bframe), scroll);
	gtk_fixed_put(GTK_FIXED(dialog_ground), bframe, 0, 55);

	
	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_size_request(box, 300, 50);
	GtkWidget * OkButton =  gtk_button_new_with_label("L??u");
	GtkWidget * CancelButton = gtk_button_new_with_label("H???y");
	gtk_widget_set_size_request(OkButton, 100, 40);
	gtk_widget_set_size_request(CancelButton, 100, 40);
	gtk_box_pack_start(GTK_BOX(box), OkButton, TRUE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(box), CancelButton, TRUE, TRUE, 2);
	gtk_fixed_put(GTK_FIXED(dialog_ground), box, 0, 260);

	
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(editdialog))), dialog_ground,  TRUE, TRUE, 0);
	gtk_widget_set_sensitive(tframe, FALSE);
	gtk_entry_set_text(GTK_ENTRY(inputtext), gtk_entry_get_text(GTK_ENTRY(textSearch)));
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(mean), gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView)));


	data_struct pointer;
	pointer.data_array[0] = inputtext;
	pointer.data_array[1] = mean;
	pointer.data_array[2] = editdialog;
	pointer.tree = soundexTree;
	g_signal_connect(OkButton, "clicked", G_CALLBACK(edit_word), &pointer);
	g_signal_connect(CancelButton, "clicked", G_CALLBACK(destroy_c), editdialog);

	gtk_widget_show_all(editdialog);
	gtk_dialog_run(GTK_DIALOG(editdialog));
	gtk_widget_destroy(editdialog);

}

void delete_word_from_dict(char *word, BTA* soundexTree){
	int delInDictResult, delInSoundexResult;
	char a[100] = "???? x??a t??? ";
	delInDictResult = btdel(data, word);
	delInSoundexResult = btdel(soundexTree, word);
	if (delInDictResult == 0 && delInSoundexResult == 0)
		Show_message(window, GTK_MESSAGE_INFO, "Th??nh c??ng!", strcat(strcat(a, word), " kh???i t??? ??i???n"));
	else
		Show_message(window, GTK_MESSAGE_ERROR, "X???y ra l???i!", "C?? l???i b???t ng??? x???y ra.");
	gtk_entry_set_text(GTK_ENTRY(textSearch), "");
}

void show_delete_dialog(GtkWidget *widget, gpointer soundexTree){
	char word[50], meantext[10000];
	int result, rsize;
	GtkWidget *deldialog;
	BTA* tree = (BTA*)soundexTree;

	strcpy(word, gtk_entry_get_text(GTK_ENTRY(textSearch)));
	if (gtk_entry_get_text_length(GTK_ENTRY(textSearch)) == 0 ||
	        btsel(data, word, meantext, 5000, &rsize) != 0) {
		Show_message(window, GTK_MESSAGE_WARNING, "C???nh b??o:", "T??? v???a nh???p kh??ng c?? trong t??? ??i???n!");
		return;
	}
	
	deldialog = gtk_message_dialog_new(GTK_WINDOW(window),
	                                   GTK_DIALOG_DESTROY_WITH_PARENT,
	                                   GTK_MESSAGE_QUESTION,
	                                   GTK_BUTTONS_YES_NO,
	                                   "X??a: \"%s\"?", gtk_entry_get_text(GTK_ENTRY(textSearch)));
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(deldialog), "B???n th???c s??? mu???n x??a t??? \"%s\" ch????",
	        gtk_entry_get_text(GTK_ENTRY(textSearch)));

	result = gtk_dialog_run(GTK_DIALOG(deldialog));
	if (result == GTK_RESPONSE_YES)
		delete_word_from_dict(word, tree);
	gtk_widget_destroy(deldialog);
}

void separate_mean(char* mean) {
  int i = 0, j = 1;
  while (mean[j] != '\0') {
    if (mean[j] == '\\' && mean[j + 1] == 'n') {
      mean[i++] = '\n';
      j += 2;
    }
    else {
      if (i != j)
        mean[i++] = mean[j++];
      else {
        i++; j++;
      }
    }
  }
  mean[i] = '\0';
}