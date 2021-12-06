// Include
#include <memory>
#include <chrono>
#include <Windows.h>
#include <d3d11.h> // DirectX11���g�p����

// DirectX Tool Kit
#include <SimpleMath.h>        // DirectX���w�⏕���C�u����
#include <GeometricPrimitive.h>// ��{�`��`�惉�C�u����

// ���C�u�����̊֘A�t��
#pragma comment( lib, "d3d11.lib" )

// ���O���
using namespace DirectX;
using namespace SimpleMath;
using namespace std::chrono;


// �v���g�^�C�v�錾
LRESULT CALLBACK WindowProc(
    HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    // �E�B���h�E�N���X�̍쐬
    WNDCLASSEX wnd;
    ZeroMemory(&wnd, sizeof(wnd));                              // �S�Ẵ����o��0�ŏ�����
    wnd.cbSize = sizeof(WNDCLASSEX);                       // ���̍\���̌^�̃T�C�Y��n��
    wnd.style = CS_HREDRAW | CS_VREDRAW;                    // �N���X�X�^�C���̎w��
    wnd.hInstance = hInstance;                                  // �C���X�^���X�n���h�����w��
    wnd.lpszClassName = "ClassName";                                // �N���X��
    wnd.hCursor = LoadCursor(nullptr, IDC_ARROW);           // �J�[�\���`��
    wnd.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1); // �w�i�F
    wnd.lpfnWndProc = WindowProc;                                 // �E�B���h�E�v���V�[�W���̊֐��|�C���^

    // �E�B���h�E�N���X�̓o�^
    if (!RegisterClassEx(&wnd))
    {
        // �G���[
        return 0;
    }

    // �E�B���h�E�X�^�C���̌���
    // �ǂ̂悤�ȃE�B���h�E�ɂ��邩�i�ő剻�A�ŏ����A����{�^���̗L���A�T�C�Y�ύX�������邩�Ȃǁj
    const DWORD style = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME;
    const DWORD ex_style = WS_EX_OVERLAPPEDWINDOW;

    // �N���C�A���g�̈�̐ݒ�
    RECT rect = { 0L, 0L, 1280L, 720L };
    AdjustWindowRectEx(&rect, style, false, ex_style);

    // �E�B���h�E�̍쐬
    // �֐�����������ƃE�B���h�E�n���h�����߂�l�Ƃ��ĕԂ��Ă���
    HWND hWnd = CreateWindowEx(
        ex_style,               // �g���E�B���h�E�X�^�C��
        "ClassName",            // �N���X��
        "Game",                 // �E�B���h�E�^�C�g��
        style,                  // �E�B���h�E�X�^�C��
        CW_USEDEFAULT,          // �\�����WX
        CW_USEDEFAULT,          // �\�����WY
        rect.right - rect.left, // �E�B���h�E�T�C�YX
        rect.bottom - rect.top, // �E�B���h�E�T�C�YY
        nullptr,                // �e�E�B���h�E�̃n���h��
        nullptr,                // ���j���[�n���h��
        hInstance,              // �C���X�^���X�n���h��
        nullptr);              // �E�B���h�E�쐬���ʎ���|�C���^

    // �E�B���h�E�n���h�����쐬�ł�����
    if (!hWnd)
    {
        // �G���[
        return 0;
    }

    // �@�\���x���̐ݒ�
    D3D_FEATURE_LEVEL level_array[] =
    {
        D3D_FEATURE_LEVEL_11_0, // DirectX11
        D3D_FEATURE_LEVEL_10_1, // DirectX10.1
        D3D_FEATURE_LEVEL_10_0  // Directx10
    };

    // �X���b�v�`�F�[���̐ݒ�
    DXGI_SWAP_CHAIN_DESC sc;
    ZeroMemory(&sc, sizeof(sc));
    sc.Windowed = true; // �E�B���h�E���[�h
    sc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // �X���b�v�G�t�F�N�g�̐ݒ�
    sc.OutputWindow = hWnd; // �E�B���h�E�n���h���̎w��
    sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �o�b�N�o�b�t�@�̎g�p���@
    sc.BufferCount = 1U; // �o�b�N�o�b�t�@�̐�
    sc.BufferDesc.Width = 1280U; // �o�b�N�o�b�t�@�T�C�Y�i���j
    sc.BufferDesc.Height = 720U; // �o�b�N�o�b�t�@�T�C�Y�i�c�j
    sc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // �t�H�[�}�b�g
    sc.BufferDesc.RefreshRate.Numerator = 60U; // ���t���b�V�����[�g���q
    sc.BufferDesc.RefreshRate.Denominator = 1U;// ���t���b�V�����[�g����
    sc.SampleDesc.Count = 1; // �}���`�T���v�����O���x��
    sc.SampleDesc.Quality = 0; // �}���`�T���v�����O�i��
    sc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // ���[�h�ؑ֗L��

    // Direct3D�C���^�[�t�F�C�X
    ID3D11Device* d3d_device = nullptr; // �`���̍쐬��
    ID3D11DeviceContext* device_context = nullptr; // �`�搧�䓙
    IDXGISwapChain* swap_chain = nullptr; // ���ۂ̕`�擙
    D3D_FEATURE_LEVEL    level;                    // �@�\���x��

    // �@�\���x�������v�Z
    int num_feature_level = sizeof(level_array) / sizeof(level_array[0]);

    // IDXGUSwapChain�AD3DDevice�AD3DDeviceContext���܂Ƃ߂č쐬����
    if (FAILED(D3D11CreateDeviceAndSwapChain(
        nullptr,                    // DXGI�A�_�v�^�[
        D3D_DRIVER_TYPE_HARDWARE,   // �h���C�o�[�^�C�v(���_����������̂͒N��)
        // ���������s����悤�ł����D3D_DRIVER_TYPE_WARP���w��
        nullptr,                    // �\�t�g�E�F�A���X�^���C�U�[DLL�n���h��
        D3D11_CREATE_DEVICE_DEBUG,  // �t���O(Debug�@�\��L���ɂ���j
        //0, // ��Release����D3D11_CREATE_DEVICE_DEBUG���w�肹����0��n��
        level_array,                // �@�\���x��
        num_feature_level,          // ���x����
        D3D11_SDK_VERSION,          // ��ɂ��̒l
        &sc,                        // DXGI_SWAP_CHAIN_DESC�\���̃|�C���^
        &swap_chain,                // ����IDXGISwapChain�|�C���^�̕ϐ�
        &d3d_device,                // ����ID3D11Device�|�C���^�̕ϐ�
        &level,                     // �ݒ肳�ꂽ�@�\���x��
        &device_context)))       // ����ID3D11DeviceContext�|�C���^�̕ϐ�
    {
        // �G���[
        return 0;
    }

    // �o�b�N�o�b�t�@�̕`��^�[�Q�b�g���擾
    // SwapChain�������Ă���`��̈�i�L�����p�X�̂悤�Ȃ��́j�����o��
    ID3D11Texture2D* back_buffer = nullptr;
    if (FAILED(swap_chain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(&back_buffer))))
    {
        // �G���[
        return 0;
    }

    // �`��^�[�Q�b�g�E�r���[�̍쐬
    // ���o�����L�����p�X���G��`���Ƃ���ł���Ƌ�����
    ID3D11RenderTargetView* render_target_view = nullptr;
    if (FAILED(d3d_device->CreateRenderTargetView(
        back_buffer,
        nullptr,
        &render_target_view)))
    {
        // �G���[
        return 0;
    }

    // �[�x�E�X�e���V���r���[�̍쐬
    ID3D11DepthStencilView* depth_stencil_view;

    // �[�x�E�X�e���V���e�N�X�`���̍쐬
    D3D11_TEXTURE2D_DESC ds;
    ZeroMemory(&ds, sizeof(ds));
    ds.Width = 1280U; // ����
    ds.Height = 720U; // �c��
    ds.MipLevels = 1U;//�~�b�v�}�b�v���x��
    ds.ArraySize = 1U;//�z��T�C�Y
    ds.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// �[�x�t�H�[�}�b�g
    ds.Usage = D3D11_USAGE_DEFAULT;           // �o�b�t�@�̎g�p���@(�f�t�H���g)
    ds.BindFlags = D3D11_BIND_DEPTH_STENCIL;  // �[�x�E�X�e���V���Ƃ��Ďg�p
    ds.SampleDesc.Count = 1U;                 // �A���`�G�C���A�V���O�̎w��

    // �[�x�X�e���V���r���[�̍쐬
    D3D11_DEPTH_STENCIL_VIEW_DESC dsv;
    ZeroMemory(&dsv,sizeof(dsv));
    dsv.Format = ds.Format;// �t�H�[�}�b�g
    dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;// 2D�e�N�X�`���ƃA�N�Z�X

    // �[�x�X�e���V���e�N�X�`���̍쐬
    ID3D11Texture2D* depth_stencil_texture;
    if (FAILED(d3d_device->CreateTexture2D(&ds, nullptr, &depth_stencil_texture)))
    {
        // �G���[
        return 0;
    }

    // �[�x�X�e���V���r���[�̍쐬
    if (FAILED(d3d_device->CreateDepthStencilView(depth_stencil_texture, &dsv, &depth_stencil_view)))
    {
        // �G���[
        return 0;
    }

    // �`��^�[�Q�b�g�r���[���o�̓}�l�[�W���[�̕`��^�[�Q�b�g�Ƃ��Đݒ�
    device_context->OMSetRenderTargets(
        1,                   // �`��^�[�Q�b�g��
        &render_target_view, // �^�[�Q�b�g�r���[�z��
        depth_stencil_view); // �[�x�X�e���V���r���[

    // �o�b�N�o�b�t�@�̉��
    back_buffer->Release();

    // �r���[�|�[�g�̍쐬�Ɛݒ�
    D3D11_VIEWPORT vp;
    ZeroMemory(&vp, sizeof(vp));
    vp.Width = 1280.0F; // �`��̈�̕�
    vp.Height = 720.0F; // �`��̈�̍���
    vp.MinDepth = 0.0F; // �ŏ��[�x�l
    vp.MaxDepth = 1.0F; // �ő�[�x�l
    device_context->RSSetViewports(1, &vp);

    // COM�̏�����
    // ������s��Ȃ���DirectX Tool Kit�������Ă���֐��̂قƂ�ǂŎ��s����
    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
    {
        // �G���[
        return 0;
    }

    // �J�����ݒ�
    // DirectXTK�͉E����W�n�Ȃ̂Ŏ�O���{
    Vector3 eye(0.0F, 1.0F, 3.0F);
    Vector3 target(0.0F, 0.0F, 0.0F);
    Vector3 up(0.0F, 1.0F, 0.0F);

    // �r���[�s��̍쐬
    Matrix view = Matrix::CreateLookAt(eye, target, up);

    // ��ʔ䗦���v�Z
    float ratio = 1280.0F / 720.0F;

    // �ˉe�s��̍쐬
    Matrix projection = Matrix::CreatePerspectiveFieldOfView(
        XMConvertToRadians(30.0F), // ��p
        ratio,                       // ��ʔ䗦
        1.0F,                        // NEAR�N���b�v��
        100.0F);                    // FAR�N���b�v��

    //*******************************
    // �X�t�B�A
    //*******************************
    // GemetricPrimitive�N���X�ϐ��̐錾
    //std::unique_ptr<GeometricPrimitive> sphere =
    //    GeometricPrimitive::CreateSphere(
    //        device_context,// �f�o�C�X�R���e�L�X�g
    //        1.0f,// �T�C�Y
    //        16U// ������
    //        );

    //*******************************
    // Cube
    //*******************************
    /*std::unique_ptr<GeometricPrimitive> cube =
        GeometricPrimitive::CreateCube
        (
            device_context,
            1.0f,
            true
        );*/

    //********************************
    // Teapot
    //********************************
    std::unique_ptr<GeometricPrimitive> teapot =
        GeometricPrimitive::CreateTeapot(device_context, 1.0f);

    //********************************
    // Torus
    //********************************
    std::unique_ptr<GeometricPrimitive> torus =
        GeometricPrimitive::CreateTorus(
            device_context,
            1.0f
        );

    // �E�B���h�E�̕\��
    ShowWindow(hWnd, SW_SHOWNORMAL);

    // ���C�����[�v
    MSG msg = { 0 };

    // ���[�v����̂��߂̎����v��
    auto prev = std::chrono::high_resolution_clock::now();

    // ��]�p�ϐ�
    float rotation = 0.0f;

    while (msg.message != WM_QUIT)
    {
        // ���b�Z�[�W����
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // ���b�Z�[�W�����Ă�����E�B���h�E�v���V�[�W�����Ăяo��
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // ���݂̎������擾
            auto now = high_resolution_clock::now();

            // ���݂̎�������O��̎���������
            auto elapsed = duration_cast<microseconds>(now - prev);


            // ��16�~���b�ȏ�o�߂��Ă�����X�V���s��
            if (elapsed.count() > 16666LL)
            {
                // ���ݎ������ߋ��̎����Ƃ��ď㏑��
                prev = now;

                // ���[���h�s��̍쐬
                Matrix world = Matrix::CreateRotationY(XMConvertToRadians(++rotation));

                // ��ʃN���A
                float color[] = { 0.3F, 0.3F, 0.3F, 1.0F }; // RGBA��0.0�`1.0�̊ԂŎw��
                device_context->ClearRenderTargetView(render_target_view, color);
                device_context->ClearDepthStencilView(depth_stencil_view, D3D10_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0F, 0);

                //*************************************
                // �X�t�B�A�̕`��
                //*************************************
                //sphere->Draw(
                //    world,// ���[���h�s��
                //    view,// �r���[�s��
                //    projection,// �ˉe�s��
                //    Colors::Red,// �J���[
                //    nullptr,// �e�N�X�`���̎w��
                //    true// ���C���[�t���[���L��
                //);

                //***************************************
                // �L���[�u�̕`��
                //***************************************
                //cube->Draw(
                //    world,        // ���[���h���W
                //    view,         // �r���[�s��
                //    projection,   // �ˉe�s��
                //    Colors::White,// �J���[
                //    nullptr,      // �e�N�X�`���̐ݒ�
                //    false         // ���C���[�t���[��
                //);

                // �e�B�[�|�b�g�̕`��
                //teapot->Draw(world, view, projection, Colors::White, nullptr, false);

                // �g�[���X�̕`��
                torus->Draw(world, view, projection, Colors::Brown);

                // ��ʍX�V
                swap_chain->Present(1, 0);
            }
        }
    }

    // �C���^�[�t�F�[�X�̉��
    // �m�ۂ������̋t�ɉ�����Ă���
    depth_stencil_texture->Release();
    depth_stencil_view->Release();
    render_target_view->Release();
    swap_chain->Release();
    device_context->ClearState();
    device_context->Release();
    d3d_device->Release();

    return 0;
}

// �E�B���h�E�v���V�[�W��
LRESULT CALLBACK WindowProc(
    HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    // ���b�Z�[�W�ɂ�蕪��
    switch (Msg)
    {
    case WM_KEYDOWN: // �L�[�������ꂽ
        switch (wParam)
        {
        case VK_ESCAPE: // ESC�L�[�������ꂽ
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        break;

    case WM_DESTROY: // �A�v���P�[�V����������ꂽ
        PostQuitMessage(0); // WM_QUIT���b�Z�[�W�𔭐�������֐�
        break;
    }

    // �E�B���h�E�v���V�[�W�����ŏ������Ȃ��������b�Z�[�W�̃f�t�H���g�̓����
    // �����I�ɍs�Ȃ��Ă����̗�H
    return DefWindowProc(hWnd, Msg, wParam, lParam);
}