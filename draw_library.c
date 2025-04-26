#include "draw_library.h"
#include <string.h>

//Acum functie input
/*void drawTextInputBox(SDL_Renderer* renderer, TTF_Font* font, char* inputText, SDL_Rect boxRect) {
    // Draw the input box background
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // dark gray
    SDL_RenderFillRect(renderer, &boxRect);

    // Draw the border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white border
    SDL_RenderDrawRect(renderer, &boxRect);

    // Render the text inside
    SDL_Color textColor = {255, 255, 255, 255}; // white
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, inputText, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {
        boxRect.x + 5, // small padding inside the box
        boxRect.y + (boxRect.h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}*/