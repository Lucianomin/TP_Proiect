#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>


int drawButton(SDL_Renderer *renderer, SDL_Texture *button_texture, SDL_Rect button, int mouseX, int mouseY, Uint32 mouseState) {
    // Check hover state
    int hovered = (mouseX >= button.x && mouseX <= button.x + button.w &&
                   mouseY >= button.y && mouseY <= button.y + button.h);
    
    // Draw the button
    SDL_Texture *texture = hovered ? button_texture : NULL;
    SDL_RenderCopy(renderer, texture, NULL, &button);
    
    // Check click
    return hovered && (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));
}

int main() {
    // Disable game controllers which might steal events
SDL_GameControllerEventState(SDL_IGNORE);
SDL_JoystickEventState(SDL_IGNORE);
    //Button 1 test
    SDL_Rect buttonRect1 = {2, 2, 365, 58 };
    //SDL_Color normal = { 255, 255, 255, 0 };    // Abuton transparent
    //SDL_Color hover = { 255, 99, 71, 0 };    // semi-transparent
    setenv("SDL_VIDEODRIVER", "x11", 1);
   
    // Inițializare SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Eroare SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    // if (TTF_Init() == -1) {
    //     printf("Eroare TTF_Init: %s\n", TTF_GetError());
    //     // handle exit
    // }
    

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
    SDL_Surface *button_image=IMG_Load("assets/bread.jpg"); 
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
    if (drawButton(renderer, buttonTexture, buttonRect1, mouseX, mouseY, mouseState)) {
        printf("Buton apăsat!\n");
        // Optional: Add delay or visual feedback here
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
