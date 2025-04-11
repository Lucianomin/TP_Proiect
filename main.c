#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

SDL_Texture* loadTexture(const char *file, SDL_Renderer *renderer) {
    SDL_Texture *newTexture = NULL;
    newTexture = IMG_LoadTexture(renderer, file);
    if (newTexture == NULL) {
        printf("Unable to load texture: %s\n", SDL_GetError());
    }
    return newTexture;
}


// int drawButton(SDL_Renderer *renderer, SDL_Texture *button_texture, SDL_Rect button, SDL_Event *event) {
//     int mouseX, mouseY;
//     static int hovered = 0;
//     static int click_handled = 0;  // To prevent multiple triggers

//     // Get current mouse state (works even when no events are coming)
//     SDL_GetMouseState(&mouseX, &mouseY);

//     // Check hover state
//     hovered = (mouseX >= button.x && mouseX <= button.x + button.w &&
//                mouseY >= button.y && mouseY <= button.y + button.h);

//     // Draw the button (your existing texture logic)
//     SDL_Texture *texture = hovered ? button_texture : NULL;
//     SDL_RenderCopy(renderer, texture, NULL, &button);

//     // Improved click detection
//     if (event) {
//         switch (event->type) {
//             case SDL_MOUSEBUTTONDOWN:
//                 if (hovered && event->button.button == SDL_BUTTON_LEFT) {
//                     click_handled = 1;
//                     return 1;
//                 }
//                 break;
                
//             case SDL_MOUSEBUTTONUP:
//                 click_handled = 0;  // Reset on button release
//                 break;
//         }
//     }

//     return 0;
// }
int drawButton(SDL_Renderer *renderer,SDL_Texture *buton_texture ,SDL_Rect button, SDL_Event *event) {
    int mouseX, mouseY;
    static int hovered=0;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Verificăm dacă mouse-ul e peste buton
     hovered = mouseX >= button.x && mouseX <= button.x + button.w &&
                  mouseY >= button.y && mouseY <= button.y + button.h;

    // Setăm culoarea
    SDL_Texture *texture = hovered ? buton_texture : NULL;
    //SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    //SDL_RenderFillRect(renderer, &button);
    SDL_RenderCopy(renderer,texture,NULL,&button);

    // Verificăm click
    // if(isMouseSelection==true)
    //     return 1;
    if (hovered && event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        return 1; // A fost apăsat
    }
     if(event->button.button == SDL_BUTTON_LEFT && event->button.button == SDL_BUTTON_LEFT)
        return  1;
    return 0; // Nu a fost apăsat
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
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT)
                    running = 0;
            }

            // if (event.type == SDL_QUIT) {
            //     printf("Quit event received\n");
            //     break;
            // }
            // else if (event.type == SDL_MOUSEMOTION) {
            //     printf("Mouse motion: (%d,%d)\n", event.motion.x, event.motion.y);
            // }
            // else if (event.type == SDL_MOUSEBUTTONDOWN) {
            //     printf("Mouse button DOWN: %d at (%d,%d)\n", 
            //            event.button.button, event.button.x, event.button.y);
            // }
            // else if (event.type == SDL_MOUSEBUTTONUP) {
            //     printf("Mouse button UP: %d at (%d,%d)\n", 
            //            event.button.button, event.button.x, event.button.y);
            // }
            // // ... your existing code

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            ///Butoane ce desenam peste
            if (drawButton(renderer,buttonTexture ,buttonRect1, &event)) {
                printf("Buton apăsat!\n");
            }
          
            SDL_RenderPresent(renderer);
           
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
