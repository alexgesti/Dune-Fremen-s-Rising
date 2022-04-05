﻿using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace RagnarEngine
{
    class UIButton : RagnarComponent
    {
        public UIButton(UIntPtr ptr) : base(ptr)
        {
            type = ComponentType.UI_BUTTON;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern int GetButtonState();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector3 GetTextPosition();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetTextPosition(float posx,float posy);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetAlpha(float NewAlpha);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float GetAlpha();

        public extern string text
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;

            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }
       
    }
}
