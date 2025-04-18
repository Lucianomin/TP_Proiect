#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

//asta modal crops
#define WINDOW_W 800
#define WINDOW_H 600
//crops
#define MAX_CROPS 20

Uint32 cropTimers[MAX_CROPS];
bool cropPlanted[MAX_CROPS];
bool grau_e_copt = true;



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



/*void render_modal(SDL_Renderer* renderer, bool* show_modal, TTF_Font* font) {
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
                cropTimers[i] = SDL_GetTicks(); // pornește timer
                printf("Grau plantat în slotul %d!\n", i);
                break;
            }
        }
        SDL_Delay(200);
    }
    
}*/


int main() {
    // sa mearga sdl 
    SDL_GameControllerEventState(SDL_IGNORE);
    SDL_JoystickEventState(SDL_IGNORE);
    //Button 1 test
    SDL_Rect buttonRect1 = {2, 2, 365, 58 };
    SDL_Rect buttonRect2 = {300, 300, 60, 60 };
    //SDL_Color normal = { 255, 255, 255, 0 };    // Abuton transparent
    //SDL_Color hover = { 255, 99, 71, 0 };    // semi-transparent
    setenv("SDL_VIDEODRIVER", "x11", 1);
   
    // Inițializare SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Eroare SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    //pentru scrii vedem daca ave, nevoie
    if (TTF_Init() == -1) {
        printf("Eroare TTF_Init: %s\n", TTF_GetError());
        // handle exit
    }
    TTF_Font* title_font = TTF_OpenFont("assets/fonts/Arial.ttf", 18); // Larger for title

    // Inițializare SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Eroare IMG_Init: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Creare fereastră
    SDL_Window *window = SDL_CreateWindow("Joc Hay Day",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          736, 736,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Eroare fereastră: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Creare renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Eroare renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    //fac render pt button
    //SDL_Texture* buttonTexture = loadTexture("assets/bread.jpg", renderer);
    // Încărcare imagine
    SDL_Surface *button_image=IMG_Load("assets/wheat_after.png"); 
    SDL_Texture *buttonTexture= SDL_CreateTextureFromSurface(renderer,button_image);
    
    SDL_Surface *image = IMG_Load("assets/grass_background_v1.jpg");
    if (!image) {
        printf("Eroare încărcare imagine: %s\n", IMG_GetError());
    } else {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
       
        SDL_FreeSurface(image);

        // Buclă principală
        int running = 1;
        SDL_Event event;

        while (running) {
            // Get mouse state once per frame
        int mouseX, mouseY;
        Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = 0;
                }
            }
    
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
    
        // Draw button and check click
    

        // if (drawButton(renderer, buttonTexture, NULL, buttonRect1, mouseX, mouseY, mouseState)) {
        //     printf(" Buton apăsat!\n");
        //         SDL_Delay(64);  
        //         if (drawButton(renderer, buttonTexture, buttonTexture, buttonRect1, mouseX, mouseY, mouseState))
        //         {
        //             printf("planmtat");
        //         }
        //  }
        if (handleButtonWithCondition(renderer, NULL, buttonTexture, buttonTexture, buttonRect1, mouseX, mouseY, mouseState, grau_e_copt)) {
            printf("Butonul a fost apăsat și condiția e %s!\n", grau_e_copt ? "adevărată" : "falsă");
        }
        
    
    SDL_RenderPresent(renderer);
    SDL_Delay(64); // Cap at ~60 FPS
}
        SDL_DestroyTexture(texture);
    }

    // Curățare
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
