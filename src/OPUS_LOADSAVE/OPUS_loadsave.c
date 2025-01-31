//
// Created by Luksz on 2021-06-11.
//

#include "OPUS_loadsave.h"

int fscanf_note(FILE *opus_text_file, NOTE *n) {

    char c;
    if (n == NULL) {
        return 1;
    }
    n->name = (char)getc(opus_text_file);
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }
    n->acci = (char)getc(opus_text_file);
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }
    n->height = (int)(getc(opus_text_file) - 48);
    if (getc(opus_text_file) != ')') {
        printf("Opus is written incorrectly");
        exit(1);
    }

    return 0;
}
CHORD *fscanf_chord(FILE *opus_text_file) {

    CHORD *chord = NULL;
    int i;
    char c;
    chord = malloc(sizeof(*chord));

    chord->time = getc(opus_text_file) - 48;
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }
    chord->_time_dots_ = getc(opus_text_file) - 48;
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }
    chord->ssp_articulation = getc(opus_text_file) - 48;
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }
    chord->notes_number = getc(opus_text_file) - 48;
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }
    chord->X_position = 0;
    while ((c = (char)getc(opus_text_file)) != ':') {
        chord->X_position *= 10;
        chord->X_position += ((int)c - 48);
    }

    for (i = 0; i < 7; i++) {
        chord->local_serial_key[i] = (char)getc(opus_text_file);
    }
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }

    for (i = 0; i < chord->notes_number; i++) {
        if (getc(opus_text_file) != '(') {
            printf("Opus is written incorrectly");
            exit(1);
        }
        fscanf_note(opus_text_file, &chord->notes_[i]);
    }

    if (getc(opus_text_file) != ']') {
        printf("Opus is written incorrectly");
        exit(1);
    }

    return chord;
}
BAR *fscanf_bar(FILE *opus_text_file) {

    BAR *bar = NULL;
    bar = malloc(sizeof(*bar));
    char c;
    bar->X_of_start_bar = 0;
    while ((c = (char)getc(opus_text_file)) != ':') {
        bar->X_of_start_bar *= 10;
        bar->X_of_start_bar += ((int)c - 48);
    }
    bar->width_ = 0;
    while ((c = (char)getc(opus_text_file)) != ':') {
        bar->width_ *= 10;
        bar->width_ += ((int)c - 48);
    }
    bar->brace = (int)getc(opus_text_file) - 48;
    if (getc(opus_text_file) != ':') {
        printf("Opus is written incorrectly");
        exit(1);
    }

    CHORD *help_chord = NULL;
    bar->prev = NULL;
    bar->next = NULL;

    if (getc(opus_text_file) != '[') {
        printf("Opus is written incorrectly");
        exit(1);
    }

    bar->first_chord_treb = fscanf_chord(opus_text_file);
    help_chord = bar->first_chord_treb;
    help_chord->prev = NULL;
    help_chord->next = NULL;

    while ((c = (char)getc(opus_text_file)) != '|') {

        help_chord->next = fscanf_chord(opus_text_file);
        help_chord->next->prev = help_chord;
        help_chord = help_chord->next;
    }
    help_chord->next = NULL;

    if (getc(opus_text_file) != '[') {
        printf("Opus is written incorrectly");
        exit(1);
    }

    bar->first_chord_bass = fscanf_chord(opus_text_file);
    help_chord = bar->first_chord_bass;
    help_chord->prev = NULL;
    help_chord->next = NULL;
    while ((c = (char)getc(opus_text_file)) != '}') {
        help_chord->next = fscanf_chord(opus_text_file);
        help_chord->next->prev = help_chord;
        help_chord = help_chord->next;
    }
    help_chord->next = NULL;


    return bar;
}
OPUS *fscanf_opus(const char *path) {

    OPUS *opus = NULL;
    opus = malloc(sizeof(*opus));
    char c;
    int i;

    FILE *opus_text_file;
    if (path != NULL) {
        if ((opus_text_file = fopen(path, "r")) == NULL) {
            printf("Opening error\n");
            exit(1);
        }
    } else {
        if ((opus_text_file = fopen("opus.txt", "r")) == NULL) {
            printf("Opening error\n");
            exit(1);
        }
    }
    i = 0;
    while ((c = (char)getc(opus_text_file)) != ':') {
        opus->title[i] = c;
        i++;
    }
    opus->title[i] = '\0';
    i = 0;
    while ((c = (char)getc(opus_text_file)) != ':') {
        opus->author[i] = c;
        i++;
    }
    opus->author[i] = '\0';

    opus->key[0] = (char)getc(opus_text_file);
    if ((c = (char)getc(opus_text_file)) != ':') {
        opus->key[1] = c;
        getc(opus_text_file);
    } else {
        opus->key[1] = '\0';
    }

    for (i = 0; i < 7; i++) {
        opus->default_serial_key[i] = (char)getc(opus_text_file);
    }
    getc(opus_text_file);

    fscanf(opus_text_file, "%d", &(opus->time_sign[0]));
    getc(opus_text_file);
    fscanf(opus_text_file, "%d", &(opus->time_sign[1]));
    getc(opus_text_file);
    fscanf(opus_text_file, "%d", &(opus->temp));
    getc(opus_text_file);

    if (getc(opus_text_file) != '{') {
        printf("Opus is written incorrectly");
        exit(1);
    }

    BAR *help_bar = NULL;
    opus->first_BAR = fscanf_bar(opus_text_file);
    opus->first_BAR->prev = NULL;
    opus->first_BAR->next = NULL;
    help_bar = opus->first_BAR;

    while (getc(opus_text_file) == '{') {

        help_bar->next = fscanf_bar(opus_text_file);
        help_bar->next->prev = help_bar;
        help_bar = help_bar->next;
    }
    help_bar->next = NULL;

    fclose(opus_text_file);
    return opus;
}


int fprint_note(FILE *opus_text_file, NOTE *n) {

    fprintf(opus_text_file, "%c", '(');
    fprintf(opus_text_file, "%c:", n->name);
    fprintf(opus_text_file, "%c:", n->acci);
    fprintf(opus_text_file, "%d", n->height);
    fprintf(opus_text_file, "%c", ')');

    return 0;
}
int fprint_chord(FILE *opus_text_file, CHORD *chord_to_print) {

    int i;
    fprintf(opus_text_file, "%c", '[');
    ///
    fprintf(opus_text_file, "%d:", chord_to_print->time);
    fprintf(opus_text_file, "%d:", chord_to_print->_time_dots_);
    fprintf(opus_text_file, "%d:", chord_to_print->ssp_articulation);
    fprintf(opus_text_file, "%d:", chord_to_print->notes_number);
    fprintf(opus_text_file, "%d:", chord_to_print->X_position);
    for (i = 0; i < 7; i++) {
        fprintf(opus_text_file, "%c", chord_to_print->local_serial_key[i]);
    }
    fprintf(opus_text_file, "%c", ':');
    for (i = 0; i < chord_to_print->notes_number; i++) {
        fprint_note(opus_text_file, &(chord_to_print->notes_[i]));
    }
    ///
    fprintf(opus_text_file, "%c", ']');

    return 0;
}
int fprint_bar(FILE *opus_text_file, BAR *bar_to_print) {

    CHORD *help_chord = NULL;
    fprintf(opus_text_file, "%c", '{');
    fprintf(opus_text_file, "%d:", bar_to_print->X_of_start_bar);
    fprintf(opus_text_file, "%d:", bar_to_print->width_);
    fprintf(opus_text_file, "%d:", bar_to_print->brace);

    help_chord = bar_to_print->first_chord_treb;
    while (help_chord != NULL) {
        fprint_chord(opus_text_file, help_chord);
        help_chord = help_chord->next;
    }
    fprintf(opus_text_file, "%c", '|');
    help_chord = bar_to_print->first_chord_bass;
    while (help_chord != NULL) {
        fprint_chord(opus_text_file, help_chord);
        help_chord = help_chord->next;
    }
    fprintf(opus_text_file, "%c", '}');

    return 0;
}
int save_OPUS_as_TextFile(OPUS *OPUS_to_save) {

    int i;
    FILE *opus_text_file = NULL;
    if ((opus_text_file = fopen("opus.txt", "w")) == NULL) {
        printf("Nie mogę otworzyć pliku test.txt do zapisu!\n");
        exit(1);
    }

    fprintf(opus_text_file, "%s:", OPUS_to_save->title);
    fprintf(opus_text_file, "%s:", OPUS_to_save->author);
    fprintf(opus_text_file, "%c", OPUS_to_save->key[0]);
    if (OPUS_to_save->key[1] == '\0') {
        fprintf(opus_text_file, "%c", ':');
    } else {
        fprintf(opus_text_file, "%c%c", OPUS_to_save->key[1],':');
    }
    for (i = 0; i < 7; i++){
        fprintf(opus_text_file, "%c", OPUS_to_save->default_serial_key[i]);
    }
    fprintf(opus_text_file, "%c", ':');

    fprintf(opus_text_file, "%d,%d:", OPUS_to_save->time_sign[0], OPUS_to_save->time_sign[1]);
    fprintf(opus_text_file, "%d:", OPUS_to_save->temp);

    BAR *bar_to_put = OPUS_to_save->first_BAR;

    while (bar_to_put != NULL) {
        fprint_bar(opus_text_file, bar_to_put);
        bar_to_put = bar_to_put->next;
    }

    fclose (opus_text_file);
    return 0;
}


/*
    opus_text_file:

    title:author:key:serial_key:metre:temp:
    {X_of_start_bar:width_:brace:
        [time:_time_dots_:ssp_articulation:notes_number:X_positon:local_serial_key:
            (name:acci:height)(...)...(...)
        ][...
        ]...[...
        ]
        | // pipe, left hand above, right hand beneath
        [...
        ]...[...
        ]
    }{...
    }...{...
    }EOF
*/
