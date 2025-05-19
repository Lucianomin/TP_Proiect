#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <emscripten/emscripten.h>
#include <stdbool.h>
#include <stdio.h>
//bibliotexa proproe
//#include "draw_library.h"

//asta modal crops
#define WINDOW_W 800
#define WINDOW_H 600
//crops
#define MAX_CROPS 6
#define MAX_FACTORY 3
#define MAX_TOMATO 6

//Uint32 cropTimers[MAX_CROPS];
bool cropPlanted[MAX_CROPS];
bool tomatoPlanted[MAX_TOMATO];
bool factoryUsed[MAX_FACTORY];
int playerMoney=0;
int g_factory_count=0;
int g_crop_planted = 0;
int g_tomato_planted = 0;
bool grau_e_copt = true;
bool bread_factory = true;
bool barn_open = true;
bool tomato_e_copt=true;
bool shop_open = true;

//input name
char townName[32] = ""; // Buffer for town name
bool inputActive = true; // Start with input active
SDL_Rect inputBox = {60, 628, 60, 30}; // Position and size of input box

const char* symbols[] = {"7", "CHERRY", "BAR"};
const int num_symbols = 3;


SDL_Texture* symbol_textures[3] = {NULL, NULL, NULL};


//slot machine
int bet = 1;
int reels[3] = {0, 0, 0};


void spin_reels() {
    for (int i = 0; i < 3; i++) {
        reels[i] = rand() % num_symbols;
    }
}

int evaluate_win() {
    if (reels[0] == reels[1] && reels[1] == reels[2] && reels[0] == reels[2]) {
        return bet * 10;  // jackpot
    }
    return 0; // no win
}

void draw_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y) {
    SDL_Color color = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void render_slot_machine(SDL_Renderer* renderer, TTF_Font* font, bool* show_modal) {
    static bool textures_loaded = false;
    if (!textures_loaded) {
        symbol_textures[0] = IMG_LoadTexture(renderer, "assets/7_slot.png");
        symbol_textures[1] = IMG_LoadTexture(renderer, "assets/cherry.png");
        symbol_textures[2] = IMG_LoadTexture(renderer, "assets/bar.png");
        textures_loaded = true;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, NULL);

    SDL_Rect modal = {WINDOW_W / 2 - 250, WINDOW_H / 2 - 200, 500, 400};
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &modal);

    SDL_Rect title_bar = {modal.x, modal.y, modal.w, 40};
    SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
    SDL_RenderFillRect(renderer, &title_bar);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* title_surface = TTF_RenderText_Blended(font, "Casino Slot Machine", white);
    SDL_Texture* title_texture = SDL_CreateTextureFromSurface(renderer, title_surface);
    SDL_Rect title_rect = {
        modal.x + 15,
        modal.y + (title_bar.h - title_surface->h)/2,
        title_surface->w,
        title_surface->h
    };
    SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
    SDL_FreeSurface(title_surface);
    SDL_DestroyTexture(title_texture);

    SDL_Rect close_btn = {modal.x + modal.w - 35, modal.y + 10, 20, 20};
    int mx, my;
    Uint32 mouseState = SDL_GetMouseState(&mx, &my);
    bool hovered = SDL_PointInRect(&(SDL_Point){mx, my}, &close_btn);

    SDL_SetRenderDrawColor(renderer, hovered ? 220 : 255, hovered ? 80 : 100, 100, 255);
    SDL_RenderFillRect(renderer, &close_btn);

    SDL_SetRenderDrawColor(renderer, 180, 60, 60, 255);
    SDL_RenderDrawRect(renderer, &close_btn);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer,
                      close_btn.x + 5, close_btn.y + 5,
                      close_btn.x + close_btn.w - 5, close_btn.y + close_btn.h - 5);
    SDL_RenderDrawLine(renderer,
                      close_btn.x + close_btn.w - 5, close_btn.y + 5,
                      close_btn.x + 5, close_btn.y + close_btn.h - 5);

    if (hovered && mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        *show_modal = false;
        return;
    }

    SDL_Rect content_area = {modal.x, modal.y + title_bar.h, modal.w, modal.h - title_bar.h};
    SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
    SDL_RenderFillRect(renderer, &content_area);

    SDL_Rect spin_btn = {modal.x + modal.w/2 - 60, modal.y + modal.h - 50, 120, 30};
    bool spin_hovered = SDL_PointInRect(&(SDL_Point){mx, my}, &spin_btn);

    SDL_SetRenderDrawColor(renderer, spin_hovered ? 180 : 130, 200, 80, 255);
    SDL_RenderFillRect(renderer, &spin_btn);
    SDL_RenderDrawRect(renderer, &spin_btn);
    draw_text(renderer, font, "SPIN", spin_btn.x + 35, spin_btn.y + 5);

    if (spin_hovered && (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT))) {
        if (playerMoney >= bet) {
            playerMoney -= bet;
            //Mix_PlayChannel(-1, spin_sound, 0);
            spin_reels();
            int win = evaluate_win();
            playerMoney += win;
        }
    }

    //Mix_FreeChunk(spin_sound);
    //Mix_CloseAudio();
    

    char credit_text[64];
    snprintf(credit_text, sizeof(credit_text), "CREDIT: %d", playerMoney);
    draw_text(renderer, font, credit_text, modal.x + 20, modal.y + 60);

    char bet_text[64];
    snprintf(bet_text, sizeof(bet_text), "BET: %d", bet);
    draw_text(renderer, font, bet_text, modal.x + 20, modal.y + 100);

    for (int i = 0; i < 3; i++) {
        SDL_Rect dst = {modal.x + 60 + i * 120, modal.y + 180, 64, 64};
        SDL_RenderCopy(renderer, symbol_textures[reels[i]], NULL, &dst);
    }

    SDL_Rect bet_buttons[5];
    int bet_values[5] = {1, 5, 25, 50, 100};
    for (int i = 0; i < 5; i++) {
        bet_buttons[i] = (SDL_Rect){modal.x + 20 + i * 90, modal.y + 130, 80, 30};
        bool hover = SDL_PointInRect(&(SDL_Point){mx, my}, &bet_buttons[i]);
        SDL_SetRenderDrawColor(renderer, hover || bet == bet_values[i] ? 200 : 100, 100, 200, 255);
        SDL_RenderFillRect(renderer, &bet_buttons[i]);
        SDL_RenderDrawRect(renderer, &bet_buttons[i]);

        char label[16];
        snprintf(label, sizeof(label), "$%d", bet_values[i]);
        draw_text(renderer, font, label, bet_buttons[i].x + 20, bet_buttons[i].y + 5);

        if (hover && (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT))) {
            bet = bet_values[i];
        }  
        
    }
}



int drawButton(SDL_Renderer *renderer, SDL_Texture *button_texture, SDL_Texture *button_texture_2,SDL_Rect button, int mouseX, int mouseY, Uint32 mouseState) {
    // Check hover state
    int hovered = (mouseX >= button.x && mouseX <= button.x + button.w &&
                   mouseY >= button.y && mouseY <= button.y + button.h);
    
    // Draw the button
    SDL_Texture *texture = hovered ? button_texture : button_texture_2;
    SDL_RenderCopy(renderer, texture, NULL, &button);
    
    // Check click
    return hovered && (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));
}


int handleButtonWithCondition(SDL_Renderer *renderer, SDL_Texture *normalTexture, SDL_Texture *hoverTextureConditionTrue, SDL_Texture *hoverTextureConditionFalse,
    SDL_Rect button, int mouseX, int mouseY, Uint32 mouseState, bool condition) {
// Determine which hover texture to use
SDL_Texture *hoverTexture = condition ? hoverTextureConditionTrue : hoverTextureConditionFalse;

// Determine which texture to render (hover or not)
int hovered = (mouseX >= button.x && mouseX <= button.x + button.w &&
mouseY >= button.y && mouseY <= button.y + button.h);

SDL_Texture *textureToUse = hovered ? hoverTexture : normalTexture;
SDL_RenderCopy(renderer, textureToUse, NULL, &button);

// Return click state
return hovered && (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));
}



void render_modal(SDL_Renderer* renderer, bool* show_modal, TTF_Font* font) {
    // Semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, NULL);
    
    // Modal box
    SDL_Rect modal = {WINDOW_W/2-200, WINDOW_H/2-150, 400, 300};
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &modal);
    
    // Title bar (added)
    SDL_Rect title_bar = {modal.x, modal.y, modal.w, 40};
    SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
    SDL_RenderFillRect(renderer, &title_bar);
    
     // ====== REPLACE THE TITLE RECTANGLE WITH THIS TTF CODE ======
     SDL_Color white = {255, 255, 255, 255};
     SDL_Surface* title_surface = TTF_RenderText_Blended(font, "Choose crops:", white);
     SDL_Texture* title_texture = SDL_CreateTextureFromSurface(renderer, title_surface);
     SDL_Rect title_rect = {
         modal.x + 15,
         modal.y + (title_bar.h - title_surface->h)/2,
         title_surface->w,
         title_surface->h
     };
     SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
     SDL_FreeSurface(title_surface);
     SDL_DestroyTexture(title_texture);
     // ====== END OF TTF CHANGES ======
    
    // Enhanced close button with hover effect
    SDL_Rect close_btn = {modal.x + modal.w - 35, modal.y + 10, 20, 20};
    
    // Hover effect
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    bool hovered = SDL_PointInRect(&(SDL_Point){mx, my}, &close_btn);
    
    // Close button background
    SDL_SetRenderDrawColor(renderer, 
                         hovered ? 220 : 255,  // Brighter when hovered
                         hovered ? 80 : 100,
                         100,
                         255);
    SDL_RenderFillRect(renderer, &close_btn);
    
    // Close button border
    SDL_SetRenderDrawColor(renderer, 180, 60, 60, 255);
    SDL_RenderDrawRect(renderer, &close_btn);
    
    // Draw X symbol (your existing code)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, 
                     close_btn.x + 5, close_btn.y + 5,
                     close_btn.x + close_btn.w - 5, close_btn.y + close_btn.h - 5);
    SDL_RenderDrawLine(renderer,
                     close_btn.x + close_btn.w - 5, close_btn.y + 5,
                     close_btn.x + 5, close_btn.y + close_btn.h - 5);
    
    // Check close button click
    if (hovered && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        *show_modal = false;
    }
    

    // Modal content area (optional)
    SDL_Rect content_area = {modal.x, modal.y + title_bar.h, modal.w, modal.h - title_bar.h};
    SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
    SDL_RenderFillRect(renderer, &content_area);
    SDL_Rect buttonRect2={modal.x, modal.y-title_bar.h+90,60,60 };
    SDL_Surface *wheat_image=IMG_Load("assets/wheat.png"); 
    SDL_Texture *wheat_texture= SDL_CreateTextureFromSurface(renderer,wheat_image);
    Uint32 mouseState = SDL_GetMouseState(&mx, &my);
    SDL_FreeSurface(wheat_image);

    if (drawButton(renderer, wheat_texture, wheat_texture, buttonRect2, mx, my, mouseState)) {
        for (int i = 0; i < MAX_CROPS; i++) {
            if (!cropPlanted[i]) {
                cropPlanted[i] = true;
                //cropTimers[i] = SDL_GetTicks(); // pornește timer
                printf("Grau plantat în slotul %d!\n", i);
                break;
            }
        }
        SDL_Delay(200);
    }
    int wheat_count = 0;
for (int i = 0; i < MAX_CROPS; i++) {
    if (cropPlanted[i]) {
        wheat_count++;
    }
}
g_crop_planted=wheat_count;

// Create the text "x N"
char countText[20];
sprintf(countText, "x %d", wheat_count);

// Render the text
SDL_Color textColor = {255, 255, 255, 255}; // white
SDL_Surface* textSurface = TTF_RenderText_Blended(font, countText, textColor);
SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

// Set position under the wheat button
SDL_Rect textRect = {
    buttonRect2.x + buttonRect2.w/2 - textSurface->w/2, // center text horizontally
    buttonRect2.y + buttonRect2.h + 5, // just below button
    textSurface->w,
    textSurface->h
};

SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

// Clean up
SDL_FreeSurface(textSurface);
SDL_DestroyTexture(textTexture);
    
}


//function for tomatoes
void render_modal_tomato(SDL_Renderer* renderer, bool* show_modal, TTF_Font* font) {
    // Semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, NULL);
    
    // Modal box
    SDL_Rect modal = {WINDOW_W/2-200, WINDOW_H/2-150, 400, 300};
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &modal);
    
    // Title bar (added)
    SDL_Rect title_bar = {modal.x, modal.y, modal.w, 40};
    SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
    SDL_RenderFillRect(renderer, &title_bar);
    
     // ====== REPLACE THE TITLE RECTANGLE WITH THIS TTF CODE ======
     SDL_Color white = {255, 255, 255, 255};
     SDL_Surface* title_surface = TTF_RenderText_Blended(font, "Choose crops:", white);
     SDL_Texture* title_texture = SDL_CreateTextureFromSurface(renderer, title_surface);
     SDL_Rect title_rect = {
         modal.x + 15,
         modal.y + (title_bar.h - title_surface->h)/2,
         title_surface->w,
         title_surface->h
     };
     SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
     SDL_FreeSurface(title_surface);
     SDL_DestroyTexture(title_texture);
     // ====== END OF TTF CHANGES ======
    
    // Enhanced close button with hover effect
    SDL_Rect close_btn = {modal.x + modal.w - 35, modal.y + 10, 20, 20};
    
    // Hover effect
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    bool hovered = SDL_PointInRect(&(SDL_Point){mx, my}, &close_btn);
    
    // Close button background
    SDL_SetRenderDrawColor(renderer, 
                         hovered ? 220 : 255,  // Brighter when hovered
                         hovered ? 80 : 100,
                         100,
                         255);
    SDL_RenderFillRect(renderer, &close_btn);
    
    // Close button border
    SDL_SetRenderDrawColor(renderer, 180, 60, 60, 255);
    SDL_RenderDrawRect(renderer, &close_btn);
    
    // Draw X symbol (your existing code)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, 
                     close_btn.x + 5, close_btn.y + 5,
                     close_btn.x + close_btn.w - 5, close_btn.y + close_btn.h - 5);
    SDL_RenderDrawLine(renderer,
                     close_btn.x + close_btn.w - 5, close_btn.y + 5,
                     close_btn.x + 5, close_btn.y + close_btn.h - 5);
    
    // Check close button click
    if (hovered && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        *show_modal = false;
    }
    

    // Modal content area (optional)
    SDL_Rect content_area = {modal.x, modal.y + title_bar.h, modal.w, modal.h - title_bar.h};
    SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
    SDL_RenderFillRect(renderer, &content_area);
    SDL_Rect buttonRect2={modal.x, modal.y-title_bar.h+90,60,60 };
    SDL_Surface *wheat_image=IMG_Load("assets/tomato.png"); 
    SDL_Texture *wheat_texture= SDL_CreateTextureFromSurface(renderer,wheat_image);
    Uint32 mouseState = SDL_GetMouseState(&mx, &my);
    SDL_FreeSurface(wheat_image);

    if (drawButton(renderer, wheat_texture, wheat_texture, buttonRect2, mx, my, mouseState)) {
        for (int i = 0; i < MAX_TOMATO; i++) {
            if (!tomatoPlanted[i]) {
                tomatoPlanted[i] = true;
                //cropTimers[i] = SDL_GetTicks(); // pornește timer
                printf("Rosie plantata în slotul %d!\n", i);
                break;
            }
        }
        SDL_Delay(200);
    }
    int tomato_count = 0;
for (int i = 0; i < MAX_TOMATO; i++) {
    if (tomatoPlanted[i]) {
        tomato_count++;
    }
}
g_tomato_planted=tomato_count;
// Create the text "x N"
char countText[20];
sprintf(countText, "x %d", tomato_count);

// Render the text
SDL_Color textColor = {255, 255, 255, 255}; // white
SDL_Surface* textSurface = TTF_RenderText_Blended(font, countText, textColor);
SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

// Set position under the wheat button
SDL_Rect textRect = {
    buttonRect2.x + buttonRect2.w/2 - textSurface->w/2, // center text horizontally
    buttonRect2.y + buttonRect2.h + 5, // just below button
    textSurface->w,
    textSurface->h
};

SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

// Clean up
SDL_FreeSurface(textSurface);
SDL_DestroyTexture(textTexture);
    
}




//function render modal factory bread
void render_modal_factory(SDL_Renderer* renderer, bool* show_modal, TTF_Font* font) {
    // Load textures ONCE at program start, not here
    static SDL_Texture* wheat_texture = NULL;
    static SDL_Texture* wheat_texture_hover=NULL;
    static bool textures_loaded = false;
    
    // Initialize textures if not already loaded
    if (!textures_loaded) {
        SDL_Surface* wheat_image_hover = IMG_Load("assets/bread_hover.png");
        if (!wheat_image_hover) {
            printf("Failed to load bread_hover.png: %s\n", IMG_GetError());
            return;
        }
        wheat_texture_hover = SDL_CreateTextureFromSurface(renderer, wheat_image_hover);
        SDL_FreeSurface(wheat_image_hover);


        SDL_Surface* wheat_image = IMG_Load("assets/bread.png");
        if (!wheat_image) {
            printf("Failed to load bread.png: %s\n", IMG_GetError());
            return;
        }
        wheat_texture = SDL_CreateTextureFromSurface(renderer, wheat_image);
        SDL_FreeSurface(wheat_image);
        textures_loaded = true;
    }

    // Semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, NULL);
    
    // Modal box
    SDL_Rect modal = {WINDOW_W/2-200, WINDOW_H/2-150, 400, 300};
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &modal);
    
    // Title bar
    SDL_Rect title_bar = {modal.x, modal.y, modal.w, 40};
    SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
    SDL_RenderFillRect(renderer, &title_bar);
    
    // Title text
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* title_surface = TTF_RenderText_Blended(font, "Choose what to bake:", white);
    if (!title_surface) {
        printf("Failed to render title text: %s\n", TTF_GetError());
        return;
    }
    
    SDL_Texture* title_texture = SDL_CreateTextureFromSurface(renderer, title_surface);
    if (!title_texture) {
        printf("Failed to create title texture: %s\n", SDL_GetError());
        SDL_FreeSurface(title_surface);
        return;
    }
    
    SDL_Rect title_rect = {
        modal.x + 15,
        modal.y + (title_bar.h - title_surface->h)/2,
        title_surface->w,
        title_surface->h
    };
    SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
    
    // Clean up title resources
    SDL_FreeSurface(title_surface);
    SDL_DestroyTexture(title_texture);
    
    // Close button logic (your existing code remains the same)
     // Enhanced close button with hover effect
     SDL_Rect close_btn = {modal.x + modal.w - 35, modal.y + 10, 20, 20};
    
     // Hover effect
     int mx, my;
     Uint32 mouseState = SDL_GetMouseState(&mx, &my); 
     bool hovered = SDL_PointInRect(&(SDL_Point){mx, my}, &close_btn);
     
     // Close button background
     SDL_SetRenderDrawColor(renderer, 
                          hovered ? 220 : 255,  // Brighter when hovered
                          hovered ? 80 : 100,
                          100,
                          255);
     SDL_RenderFillRect(renderer, &close_btn);
     
     // Close button border
     SDL_SetRenderDrawColor(renderer, 180, 60, 60, 255);
     SDL_RenderDrawRect(renderer, &close_btn);
     
     // Draw X symbol (your existing code)
     SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
     SDL_RenderDrawLine(renderer, 
                      close_btn.x + 5, close_btn.y + 5,
                      close_btn.x + close_btn.w - 5, close_btn.y + close_btn.h - 5);
     SDL_RenderDrawLine(renderer,
                      close_btn.x + close_btn.w - 5, close_btn.y + 5,
                      close_btn.x + 5, close_btn.y + close_btn.h - 5);
     
     // Check close button click
     if (hovered && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
         *show_modal = false;
     }


    
    // Modal content area
    SDL_Rect content_area = {modal.x, modal.y + title_bar.h, modal.w, modal.h - title_bar.h};
    SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
    SDL_RenderFillRect(renderer, &content_area);
    
    // Button position
    SDL_Rect buttonRect2 = {modal.x, modal.y - title_bar.h + 90, 60, 60};
    
    // Draw button
    if (wheat_texture && drawButton(renderer, wheat_texture_hover, wheat_texture, buttonRect2, mx, my, mouseState)) {
        // Count available planted crops
        int available_crops = 0;
        for (int i = 0; i < MAX_CROPS; i++) {
            if (cropPlanted[i]) {
                available_crops++;
            }
        }
    
        // Check if there are at least 2 crops
        if (available_crops >= 2) {
            for (int i = 0; i < MAX_FACTORY; i++) {
                if (!factoryUsed[i]) {
                    factoryUsed[i] = true;
                    printf("Paine pusa la copt %d!\n", i);
    
                    // Consume (set to false) 2 wheat crops
                    int crops_consumed = 0;
                    for (int j = 0; j < MAX_CROPS && crops_consumed < 2; j++) {
                        if (cropPlanted[j]) {
                            cropPlanted[j] = false;
                            crops_consumed++;
                        }
                    }
                    break; // only bake one bread at a time
                }
            }
        } else {
            printf("Nu ai destul grau pentru a coace paine!\n");
        }
    }
    
    
    // Count active factories
    int factory_count = 0;
    for (int i = 0; i < MAX_FACTORY; i++) {
        if (factoryUsed[i]) {
            factory_count++;
        }
    }
    g_factory_count=factory_count;
    // Render count text
    char countText[20];
    snprintf(countText, sizeof(countText), "x %d", factory_count);
    
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, countText, white);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture) {
            SDL_Rect textRect = {
                buttonRect2.x + buttonRect2.w/2 - textSurface->w/2,
                buttonRect2.y + buttonRect2.h + 5,
                textSurface->w,
                textSurface->h
            };
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_FreeSurface(textSurface);
    }
}

void render_modal_barn(SDL_Renderer* renderer, bool* show_modal, TTF_Font* font) {
    // Load textures ONCE at program start, not here
    static SDL_Texture* wheat_texture = NULL;
    static SDL_Texture* corn_texture = NULL;
    static SDL_Texture* tomato_texture = NULL;
    static SDL_Texture* corn_texture_hover = NULL;
    static bool textures_loaded = false;
    
    if (!textures_loaded) {
        SDL_Surface* wheat_image_hover = IMG_Load("assets/bread_hover.png");
        if (!wheat_image_hover) {
            printf("Failed to load bread_hover.png: %s\n", IMG_GetError());
            return;
        }
        
        SDL_FreeSurface(wheat_image_hover);

        SDL_Surface* wheat_image = IMG_Load("assets/bread.png");
        if (!wheat_image) {
            printf("Failed to load bread.png: %s\n", IMG_GetError());
            return;
        }
        wheat_texture = SDL_CreateTextureFromSurface(renderer, wheat_image);
        SDL_FreeSurface(wheat_image);

        SDL_Surface* corn_image_hover = IMG_Load("assets/wheat.png"); // <-- new
        if (!corn_image_hover) {
            printf("Failed to load corn_hover.png: %s\n", IMG_GetError());
            return;
        }
        corn_texture_hover = SDL_CreateTextureFromSurface(renderer, corn_image_hover);
        SDL_FreeSurface(corn_image_hover);

        SDL_Surface* corn_image = IMG_Load("assets/wheat.png"); // <-- new
        if (!corn_image) {
            printf("Failed to load wheat.png: %s\n", IMG_GetError());
            return;
        }
        corn_texture = SDL_CreateTextureFromSurface(renderer, corn_image);
        SDL_FreeSurface(corn_image);

        SDL_Surface* tomato_image = IMG_Load("assets/tomato.png"); // <-- new
        if (!tomato_image) {
            printf("Failed to load tomato.png: %s\n", IMG_GetError());
            return;
        }
        tomato_texture = SDL_CreateTextureFromSurface(renderer, tomato_image);
        SDL_FreeSurface(tomato_image);

        textures_loaded = true;
    }

    // Semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, NULL);
    
    // Modal box
    SDL_Rect modal = {WINDOW_W/2-200, WINDOW_H/2-150, 400, 300};
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &modal);
    
    // Title bar
    SDL_Rect title_bar = {modal.x, modal.y, modal.w, 40};
    SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
    SDL_RenderFillRect(renderer, &title_bar);

    // Title text
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* title_surface = TTF_RenderText_Blended(font, "Choose what to bake:", white);
    if (!title_surface) {
        printf("Failed to render title text: %s\n", TTF_GetError());
        return;
    }
    
    SDL_Texture* title_texture = SDL_CreateTextureFromSurface(renderer, title_surface);
    if (!title_texture) {
        printf("Failed to create title texture: %s\n", SDL_GetError());
        SDL_FreeSurface(title_surface);
        return;
    }
    
    SDL_Rect title_rect = {
        modal.x + 15,
        modal.y + (title_bar.h - title_surface->h)/2,
        title_surface->w,
        title_surface->h
    };
    SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
    
    SDL_FreeSurface(title_surface);
    SDL_DestroyTexture(title_texture);
    
    // Close button
    SDL_Rect close_btn = {modal.x + modal.w - 35, modal.y + 10, 20, 20};
    int mx, my;
    Uint32 mouseState = SDL_GetMouseState(&mx, &my); 
    bool hovered = SDL_PointInRect(&(SDL_Point){mx, my}, &close_btn);

    SDL_SetRenderDrawColor(renderer, 
                          hovered ? 220 : 255,
                          hovered ? 80 : 100,
                          100,
                          255);
    SDL_RenderFillRect(renderer, &close_btn);
    
    SDL_SetRenderDrawColor(renderer, 180, 60, 60, 255);
    SDL_RenderDrawRect(renderer, &close_btn);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, 
                      close_btn.x + 5, close_btn.y + 5,
                      close_btn.x + close_btn.w - 5, close_btn.y + close_btn.h - 5);
    SDL_RenderDrawLine(renderer,
                      close_btn.x + close_btn.w - 5, close_btn.y + 5,
                      close_btn.x + 5, close_btn.y + close_btn.h - 5);

    if (hovered && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        *show_modal = false;
    }

    // Content area
    SDL_Rect content_area = {modal.x, modal.y + title_bar.h, modal.w, modal.h - title_bar.h};
    SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
    SDL_RenderFillRect(renderer, &content_area);

    // Buttons
    SDL_Rect buttonRect2 = {modal.x, modal.y - title_bar.h + 90, 60, 60};
    SDL_Rect buttonRect3 = {modal.x + 70, modal.y - title_bar.h + 90, 60, 60}; // Little spacing between buttons
    SDL_Rect buttonRect4 = {modal.x + 140, modal.y - title_bar.h + 90, 60, 60};
    // First Button (Bread/Wheat)
    drawButton(renderer, wheat_texture, wheat_texture, buttonRect2, mx, my, mouseState);
    

    // Second Button (Corn/Other)
    drawButton(renderer, corn_texture_hover, corn_texture, buttonRect3, mx, my, mouseState);
  
    drawButton(renderer, tomato_texture, tomato_texture, buttonRect4, mx, my, mouseState);
    // Count active factories
     int factory_count = 0;
    for (int i = 0; i < MAX_FACTORY; i++) {
        if (factoryUsed[i]) {
            factory_count++;
        }
    }
   g_factory_count=factory_count;

    // Render count text under first button
    char countText[20];
    snprintf(countText, sizeof(countText), "x %d", factory_count);
    
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, countText, white);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture) {
            SDL_Rect textRect = {
                buttonRect2.x + buttonRect2.w/2 - textSurface->w/2,
                buttonRect2.y + buttonRect2.h + 5,
                textSurface->w,
                textSurface->h
            };
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_FreeSurface(textSurface);
    } 
    
    int crop_planted=0;
    for (int i = 0; i < MAX_CROPS; i++) {
        if (cropPlanted[i]) {
            crop_planted++;
        }
    }
    g_crop_planted=crop_planted;
    // Render count text under second button (crops planted count)
    char cropCountText[20];
    snprintf(cropCountText, sizeof(cropCountText), "x %d", crop_planted);

    SDL_Surface* textSurface2 = TTF_RenderText_Blended(font, cropCountText, white);
    if (textSurface2) {
        SDL_Texture* textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
        if (textTexture2) {
            SDL_Rect textRect2 = {
                buttonRect3.x + buttonRect3.w/2 - textSurface2->w/2,
                buttonRect3.y + buttonRect3.h + 5,
                textSurface2->w,
                textSurface2->h
            };
            SDL_RenderCopy(renderer, textTexture2, NULL, &textRect2);
            SDL_DestroyTexture(textTexture2);
        }
        SDL_FreeSurface(textSurface2);
    }


    //pentru tomato
    int tomato_planted=0;
    for (int i = 0; i < MAX_TOMATO; i++) {
        if (tomatoPlanted[i]) {
            tomato_planted++;
        }
    }
    g_tomato_planted=tomato_planted;
    // Render count text under second button (crops planted count)
    char tomatoCountText[20];
    snprintf(tomatoCountText, sizeof(tomatoCountText), "x %d", tomato_planted);

    SDL_Surface* textSurface3 = TTF_RenderText_Blended(font, tomatoCountText, white);
    if (textSurface3) {
        SDL_Texture* textTexture3 = SDL_CreateTextureFromSurface(renderer, textSurface3);
        if (textTexture3) {
            SDL_Rect textRect3 = {
                buttonRect4.x + buttonRect4.w/2 - textSurface3->w/2,
                buttonRect4.y + buttonRect4.h + 5,
                textSurface3->w,
                textSurface3->h
            };
            SDL_RenderCopy(renderer, textTexture3, NULL, &textRect3);
            SDL_DestroyTexture(textTexture3);
        }
        SDL_FreeSurface(textSurface3);
    }
}



//aici functie de sell mini SHOP
//<<__
//<<--
void render_modal_shop(SDL_Renderer* renderer, bool* show_modal, TTF_Font* font) {
    static SDL_Texture* wheat_texture = NULL;
    static SDL_Texture* corn_texture = NULL;
    static SDL_Texture* tomato_texture = NULL;
    static bool textures_loaded = false;

    if (!textures_loaded) {
        SDL_Surface* wheat_image = IMG_Load("assets/bread.png");
        wheat_texture = wheat_image ? SDL_CreateTextureFromSurface(renderer, wheat_image) : NULL;
        SDL_FreeSurface(wheat_image);

        SDL_Surface* corn_image = IMG_Load("assets/wheat.png");
        corn_texture = corn_image ? SDL_CreateTextureFromSurface(renderer, corn_image) : NULL;
        SDL_FreeSurface(corn_image);

        SDL_Surface* tomato_image = IMG_Load("assets/tomato.png");
        tomato_texture = tomato_image ? SDL_CreateTextureFromSurface(renderer, tomato_image) : NULL;
        SDL_FreeSurface(tomato_image);

        textures_loaded = true;
    }

    // Overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, NULL);

    SDL_Rect modal = {WINDOW_W/2-200, WINDOW_H/2-150, 400, 300};
    SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
    SDL_RenderFillRect(renderer, &modal);

    SDL_Color white = {255, 255, 255, 255};

    // Title
    SDL_Surface* title_surf = TTF_RenderText_Blended(font, "Mini Shop - Sell Items", white);
    SDL_Texture* title_tex = SDL_CreateTextureFromSurface(renderer, title_surf);
    SDL_Rect title_rect = {modal.x + 20, modal.y + 10, title_surf->w, title_surf->h};
    SDL_RenderCopy(renderer, title_tex, NULL, &title_rect);
    SDL_FreeSurface(title_surf);
    SDL_DestroyTexture(title_tex);

    // Money display
    char moneyText[32];
    snprintf(moneyText, sizeof(moneyText), "Money: $%d", playerMoney);
    SDL_Surface* money_surf = TTF_RenderText_Blended(font, moneyText, white);
    SDL_Texture* money_tex = SDL_CreateTextureFromSurface(renderer, money_surf);
    SDL_Rect money_rect = {modal.x + modal.w - money_surf->w - 35, modal.y + 10, money_surf->w, money_surf->h};
    SDL_RenderCopy(renderer, money_tex, NULL, &money_rect);
    SDL_FreeSurface(money_surf);
    SDL_DestroyTexture(money_tex);

    // Items setup
    SDL_Rect slots[3] = {
        {modal.x + 40, modal.y + 70, 80, 80},
        {modal.x + 160, modal.y + 70, 80, 80},
        {modal.x + 280, modal.y + 70, 80, 80}
    };

    SDL_Texture* textures[3] = {wheat_texture, corn_texture, tomato_texture};
    int* counts[3] = {&g_factory_count, &g_crop_planted, &g_tomato_planted}; // Replace with your actual int vars
    int prices[3] = {10, 15, 20}; // You define prices here

    int mx, my;
    Uint32 mouseState = SDL_GetMouseState(&mx, &my);

    for (int i = 0; i < 3; i++) {
        SDL_RenderDrawRect(renderer, &slots[i]);

        if (textures[i])
            SDL_RenderCopy(renderer, textures[i], NULL, &slots[i]);

        // Count text
        char countBuf[16];
        snprintf(countBuf, sizeof(countBuf), "x%d", *counts[i]);
        SDL_Surface* count_surf = TTF_RenderText_Blended(font, countBuf, white);
        SDL_Texture* count_tex = SDL_CreateTextureFromSurface(renderer, count_surf);
        SDL_Rect count_rect = {slots[i].x + 20, slots[i].y + 85, count_surf->w, count_surf->h};
        SDL_RenderCopy(renderer, count_tex, NULL, &count_rect);
        SDL_FreeSurface(count_surf);
        SDL_DestroyTexture(count_tex);

        // Sell button
        SDL_Rect sellBtn = {slots[i].x, slots[i].y + 110, 80, 25};
        bool hover = SDL_PointInRect(&(SDL_Point){mx, my}, &sellBtn);

        SDL_SetRenderDrawColor(renderer, hover ? 120 : 90, 180, 90, 255);
        SDL_RenderFillRect(renderer, &sellBtn);
        SDL_RenderDrawRect(renderer, &sellBtn);

        char sellText[32];
        snprintf(sellText, sizeof(sellText), "Sell ($%d)", prices[i]);
        SDL_Surface* sell_surf = TTF_RenderText_Blended(font, sellText, white);
        SDL_Texture* sell_tex = SDL_CreateTextureFromSurface(renderer, sell_surf);
        SDL_Rect sell_rect = {
            sellBtn.x + (sellBtn.w - sell_surf->w)/2,
            sellBtn.y + 3,
            sell_surf->w,
            sell_surf->h
        };
        SDL_RenderCopy(renderer, sell_tex, NULL, &sell_rect);
        SDL_FreeSurface(sell_surf);
        SDL_DestroyTexture(sell_tex);

        // Click handling
        
if (hover && (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT))) {
    if (*counts[i] > 0) {
        (*counts[i])--;
        playerMoney += prices[i];

        // Update boolean arrays
        if (i == 0) { // Bread / factoryUsed
            for (int j = MAX_FACTORY - 1; j >= 0; j--) {
                if (factoryUsed[j]) {
                    factoryUsed[j] = false;
                    break;
                }
            }
        }
        else if (i == 1) { // Wheat / cropPlanted
            for (int j = MAX_CROPS - 1; j >= 0; j--) {
                if (cropPlanted[j]) {
                    cropPlanted[j] = false;
                    break;
                }
            }
        }
        else if (i == 2) { // Tomato / tomatoPlanted
            for (int j = MAX_TOMATO - 1; j >= 0; j--) {
                if (tomatoPlanted[j]) {
                    tomatoPlanted[j] = false;
                    break;
                }
            }
        }
    }
}

    }

    // Close button
SDL_Rect close_btn = {modal.x + modal.w - 30, modal.y + 10, 20, 20};
bool hovered = SDL_PointInRect(&(SDL_Point){mx, my}, &close_btn);

// Draw red background
SDL_SetRenderDrawColor(renderer, hovered ? 200 : 160, 60, 60, 255);
SDL_RenderFillRect(renderer, &close_btn);

// ✅ Now set color to white for the "X"
SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
SDL_RenderDrawLine(renderer, close_btn.x + 4, close_btn.y + 4,
                   close_btn.x + 16, close_btn.y + 16);
SDL_RenderDrawLine(renderer, close_btn.x + 16, close_btn.y + 4,
                   close_btn.x + 4, close_btn.y + 16);

// Handle click
if (hovered && (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT))) {
    *show_modal = false;
}

}






// Function to handle text input events
void handleTextInput(SDL_Event* event) {
    if (!inputActive) return;

    if (event->type == SDL_TEXTINPUT) {
        // Add character to town name if there's space
        if (strlen(townName) < sizeof(townName) - 1) {
            strcat(townName, event->text.text);
        }
    }
    else if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_BACKSPACE && strlen(townName) > 0) {
            // Remove last character
            townName[strlen(townName) - 1] = '\0';
        }
        else if (event->key.keysym.sym == SDLK_RETURN) {
            // Start game with entered town name
            if (strlen(townName) > 0) {
                inputActive = false;
                
            }
        }
    }
}

// Modified draw function with cursor
void drawTextInputBox(SDL_Renderer* renderer, TTF_Font* font) {
    // Draw background
    SDL_SetRenderDrawColor(renderer, 102, 1, 19, 0);
    SDL_RenderFillRect(renderer, &inputBox);

    // Draw border
    //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    //SDL_RenderDrawRect(renderer, &inputBox);

    // Render text
    SDL_Color textColor = {255, 255, 255, 255};
    const char* displayText = strlen(townName) > 0 ? townName : "Name...";
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, displayText, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {
        inputBox.x + 5,
        inputBox.y + (inputBox.h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };

    // Make sure text doesn't overflow the box
    if (textRect.w > inputBox.w - 10) {
        textRect.w = inputBox.w - 10;
    }

    SDL_RenderCopyEx(renderer, textTexture, NULL, &textRect, -16.0, NULL, SDL_FLIP_NONE);

    // Draw blinking cursor if active
    if (inputActive && (SDL_GetTicks() / 500) % 2 == 0) {
        SDL_Rect cursorRect = {
            inputBox.x + 5 + (strlen(townName) > 0 ? textRect.w : 0),
            inputBox.y + 5,
            2,
            inputBox.h - 10
        };
        SDL_RenderFillRect(renderer, &cursorRect);
    }

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}


// Variabile globale
SDL_Renderer* renderer;
SDL_Texture *texture, *buttonTexture, *buttonTextureFactory, *buttonTextureBarn, *buttonTextureTomato;
TTF_Font* title_font;

// Buclă principală
bool show_modal=false;
bool show_modal_factory = false;
bool show_modal_barn = false;
bool show_modal_tomato=false;
bool show_modal_shop=false;
bool show_modal_pacane=false;
char inputText[256] = "";

// Rect-uri pentru butoane
 //Button 1 test
 SDL_Rect buttonRect1 = {2, 2, 365, 58 };
 SDL_Rect buttonRect2 = {440, 20, 150, 98 };
 SDL_Rect buttonRect3 = {280, 560, 170, 108};
 SDL_Rect buttonRect4 = {2, 70, 365, 60};
 SDL_Rect buttonRect5= {430, 480, 170, 108};
 SDL_Rect buttonRect6={130, 250, 170, 108};

// Funcția principală de redare pentru Emscripten
void main_loop() {
    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return;
        handleTextInput(&event);
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    SDL_Surface* wheat_image_hover = IMG_Load("assets/bread_hover.png");

    SDL_Surface *button_image=IMG_Load("assets/wheat_after.png"); 
    SDL_Texture *buttonTexture= SDL_CreateTextureFromSurface(renderer,button_image);

    SDL_Surface *button_imageTomato=IMG_Load("assets/tomato_hover.png"); 
    SDL_Texture *buttonTextureTomato= SDL_CreateTextureFromSurface(renderer,button_imageTomato);

    SDL_Surface *button_image_factory=IMG_Load("assets/bread_factory.png");
    SDL_Texture *buttonTextureFactory=SDL_CreateTextureFromSurface(renderer,button_image_factory);

    SDL_Surface *button_image_barn=IMG_Load("assets/barn_image.png");
    SDL_Texture *buttonTextureBarn=SDL_CreateTextureFromSurface(renderer,button_image_barn);


    if (inputText[0]) drawTextInputBox(renderer, title_font);
//buton pacane
if(!show_modal_pacane)
{
    if (handleButtonWithCondition(renderer, NULL, buttonTextureBarn, buttonTextureBarn, buttonRect6, mouseX, mouseY, mouseState, barn_open))
        {
            show_modal_pacane=true;
            printf("Butonul a fost apăsat și condiția e!\n");
  
        }
}
if (show_modal_pacane) {
   
        render_slot_machine(renderer, title_font, &show_modal_pacane);
    
      }



//buton BARN
if(!show_modal_barn)
{
    if (handleButtonWithCondition(renderer, NULL, buttonTextureBarn, buttonTextureBarn, buttonRect3, mouseX, mouseY, mouseState, barn_open))
        {
            show_modal_barn=true;
            printf("Butonul a fost apăsat și condiția e!\n");
  
        }
}
if (show_modal_barn) {
    render_modal_barn(renderer, &show_modal_barn, title_font);  // <<<<<< CALL IT CORRECTLY
}

  //buton SHOP
  if(!show_modal_shop)
  {
      if (handleButtonWithCondition(renderer, NULL, buttonTextureBarn, buttonTextureBarn, buttonRect5, mouseX, mouseY, mouseState, shop_open))
          {
              show_modal_shop=true;
              printf("Butonul a fost apăsat și condiția e!\n");
    
          }
  }
  if (show_modal_shop) {
      render_modal_shop(renderer, &show_modal_shop, title_font);  // <<<<<< CALL IT CORRECTLY
  }


//zona pentru modal grauu
if (!show_modal) { 
    if (handleButtonWithCondition(renderer, NULL, buttonTexture, buttonTexture, buttonRect1, mouseX, mouseY, mouseState, grau_e_copt)) {
        show_modal = true;   // <<<<<< OPEN MOD
        printf("Butonul a fost apăsat și condiția e %s!\n", grau_e_copt ? "adevărată" : "falsă");
    }
}
//aici sunt rosii plantatie
if (!show_modal_tomato) { 
    if (handleButtonWithCondition(renderer, NULL, buttonTextureTomato, buttonTextureTomato, buttonRect4, mouseX, mouseY, mouseState, tomato_e_copt)) {
        show_modal_tomato = true;   // <<<<<< OPEN MODAL
        printf("Butonul a fost apăsat și condiția e %s!\n", tomato_e_copt ? "adevărată" : "falsă");
    }
}

if (show_modal) {
    render_modal(renderer, &show_modal, title_font);  // <<<<<< CALL IT CORRECTLY
}
if (show_modal_tomato) {
    render_modal_tomato(renderer, &show_modal_tomato, title_font);  // <<<<<< CALL IT CORRECTLY
}


    // Buton FABRICĂ
    if (!show_modal_factory) {
        if (handleButtonWithCondition(renderer, NULL, buttonTextureFactory, buttonTextureFactory, buttonRect2, mouseX, mouseY, mouseState, bread_factory)) {
            show_modal_factory = true;
        }
    }

    if (show_modal_factory) render_modal_factory(renderer, &show_modal_factory, title_font);

    SDL_RenderPresent(renderer);
}

// Funcția main
int main() {
    // Dezactivare controllere
    SDL_GameControllerEventState(SDL_IGNORE);
    SDL_JoystickEventState(SDL_IGNORE);

    // Inițializare SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Eroare SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    // Inițializare TTF
    if (TTF_Init() == -1) {
        printf("Eroare TTF_Init: %s\n", TTF_GetError());
        return 1;
    }

    // Inițializare SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Eroare IMG_Init: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Creare fereastră
    SDL_Window* window = SDL_CreateWindow("Joc Hay Day", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 736, 736, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Eroare fereastră: %s\n", SDL_GetError());
        IMG_Quit(); SDL_Quit();
        return 1;
    }

    // Creare renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Eroare renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window); IMG_Quit(); SDL_Quit();
        return 1;
    }

    // Încărcare texturi
    SDL_Surface* image = IMG_Load("assets/grass_background_v2.jpg");
    if (!image) {
        printf("Eroare încărcare imagine: %s\n", IMG_GetError());
        return 1;
    }
    texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

   


    // Font
    title_font = TTF_OpenFont("assets/fonts/Arial.ttf", 18);

    // Activare input text
    SDL_StartTextInput();

    // Bucla principală controlată de Emscripten
    emscripten_set_main_loop(main_loop, 0, 1);

    return 0;
}
