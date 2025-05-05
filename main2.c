#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <emscripten/emscripten.h>
//bibliotexa proproe
//#include "draw_library.h"

//asta modal crops
#define WINDOW_W 800
#define WINDOW_H 600
//crops
#define MAX_CROPS 6
#define MAX_FACTORY 3

//Uint32 cropTimers[MAX_CROPS];
bool cropPlanted[MAX_CROPS];
bool factoryUsed[MAX_FACTORY];
bool grau_e_copt = true;
bool bread_factory = true;
bool barn_open = true;


//input name
char townName[32] = ""; // Buffer for town name
bool inputActive = true; // Start with input active
SDL_Rect inputBox = {60, 628, 60, 30}; // Position and size of input box


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

    // First Button (Bread/Wheat)
    drawButton(renderer, wheat_texture, wheat_texture, buttonRect2, mx, my, mouseState);
    

    // Second Button (Corn/Other)
    drawButton(renderer, corn_texture_hover, corn_texture, buttonRect3, mx, my, mouseState);
  

    // Count active factories
     int factory_count = 0;
    for (int i = 0; i < MAX_FACTORY; i++) {
        if (factoryUsed[i]) {
            factory_count++;
        }
    }
   

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
SDL_Texture *texture, *buttonTexture, *buttonTextureFactory;
TTF_Font* title_font;

bool show_modal = false, show_modal_factory = false, show_modal_barn = false;
char inputText[256] = "";

// Rect-uri pentru butoane
SDL_Rect buttonRect1 = { 2, 2, 365, 58 };
SDL_Rect buttonRect2 = { 440, 20, 150, 98 };
SDL_Rect buttonRect3 = { 400, 600, 150, 98 };
SDL_Rect buttonRect4 = { 2, 60, 365, 58};

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

    if (inputText[0]) drawTextInputBox(renderer, title_font);

    // Buton BARN
    if (!show_modal_barn) {
        if (handleButtonWithCondition(renderer, NULL, buttonTextureFactory, buttonTextureFactory, buttonRect3, mouseX, mouseY, mouseState, barn_open)) {
            show_modal_barn = true;
        }
    }

    if (show_modal_barn) render_modal_barn(renderer, &show_modal_barn, title_font);

    // Buton GRÂU
    if (!show_modal) {
        if (handleButtonWithCondition(renderer, NULL, buttonTexture, buttonTexture, buttonRect1, mouseX, mouseY, mouseState, grau_e_copt)) {
            show_modal = true;
        }
    }
    if (!show_modal) {
        if (handleButtonWithCondition(renderer, NULL, buttonTexture, buttonTexture, buttonRect4, mouseX, mouseY, mouseState, grau_e_copt)) {
            show_modal = true;
        }
    }

    if (show_modal) render_modal(renderer, &show_modal, title_font);

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

    SDL_Surface* button_image = IMG_Load("assets/wheat_after.png");
    buttonTexture = SDL_CreateTextureFromSurface(renderer, button_image);

    SDL_Surface* button_image_factory = IMG_Load("assets/bread_factory.png");
    buttonTextureFactory = SDL_CreateTextureFromSurface(renderer, button_image_factory);

    // Font
    title_font = TTF_OpenFont("assets/fonts/Arial.ttf", 18);

    // Activare input text
    SDL_StartTextInput();

    // Bucla principală controlată de Emscripten
    emscripten_set_main_loop(main_loop, 0, 1);

    return 0;
}
